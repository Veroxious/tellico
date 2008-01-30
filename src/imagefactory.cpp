/***************************************************************************
    copyright            : (C) 2003-2006 by Robby Stephenson
    email                : robby@periapsis.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of version 2 of the GNU General Public License as  *
 *   published by the Free Software Foundation;                            *
 *                                                                         *
 ***************************************************************************/

#include "imagefactory.h"
#include "image.h"
#include "document.h"
#include "filehandler.h"
#include "tellico_utils.h"
#include "tellico_kernel.h"
#include "core/tellico_config.h"
#include "tellico_debug.h"

#include <ktempdir.h>
#include <kapplication.h>
#include <kimageeffect.h>

#include <qfile.h>
#include <qdir.h>

#define RELEASE_IMAGES

using Tellico::ImageFactory;

bool ImageFactory::s_needInit = true;
const Tellico::Data::Image ImageFactory::s_null;

QDict<Tellico::Data::Image> ImageFactory::s_imageDict;
// since most images get turned into pixmaps quickly, use 10 megs
// for images and 10 megs for pixmaps
QCache<Tellico::Data::Image> ImageFactory::s_imageCache(10 * 1024 * 1024);
QCache<QPixmap> ImageFactory::s_pixmapCache(10 * 1024 * 1024);
// this image info map is just for big images that don't fit
// in the cache, so that don't have to be continually reloaded to get info
QMap<QString, Tellico::Data::ImageInfo> ImageFactory::s_imageInfoMap;
Tellico::StringSet ImageFactory::s_imagesInTmpDir;
Tellico::StringSet ImageFactory::s_imagesToRelease;
KTempDir* ImageFactory::s_tmpDir = 0;
QString ImageFactory::s_localDir;

void ImageFactory::init() {
  if(!s_needInit) {
    return;
  }
  s_imageDict.setAutoDelete(true);
  s_imageCache.setAutoDelete(true);
  s_imageCache.setMaxCost(Config::imageCacheSize());
  s_pixmapCache.setAutoDelete(true);
  s_needInit = false;
}

QString ImageFactory::tempDir() {
  if(!s_tmpDir) {
    s_tmpDir = new KTempDir();
    s_tmpDir->setAutoDelete(true);
  }
  return s_tmpDir->name();
}

QString ImageFactory::dataDir() {
  static const QString dataDir = Tellico::saveLocation(QString::fromLatin1("data/"));
  return dataDir;
}

QString ImageFactory::localDir() {
  if(s_localDir.isEmpty()) {
    return dataDir();
  }
  return s_localDir;
}

QString ImageFactory::addImage(const KURL& url_, bool quiet_, const KURL& refer_, bool link_) {
  return addImageImpl(url_, quiet_, refer_, link_).id();
}

const Tellico::Data::Image& ImageFactory::addImageImpl(const KURL& url_, bool quiet_, const KURL& refer_, bool link_) {
  if(url_.isEmpty() || !url_.isValid()) {
    return s_null;
  }
//  myLog() << "ImageFactory::addImageImpl(KURL) - " << url_.prettyURL() << endl;
  Data::Image* img = refer_.isEmpty()
                     ? FileHandler::readImageFile(url_, quiet_)
                     : FileHandler::readImageFile(url_, quiet_, refer_);
  if(!img) {
    myLog() << "ImageFactory::addImageImpl() - image not found: " << url_.prettyURL() << endl;
    return s_null;
  }
  if(img->isNull()) {
    delete img;
    return s_null;
  }

  if(link_) {
    img->setLinkOnly(true);
    img->setID(url_.url());
  }

  if(hasImage(img->id())) {
//    myDebug() << "### ImageFactory::addImageImpl() - hasImage() is true!" << endl;
    const Data::Image& img2 = imageById(img->id());
    if(!img2.isNull()) {
      delete img;
      return img2;
    }
  }

  s_imageDict.insert(img->id(), img);
  s_imageInfoMap.insert(img->id(), Data::ImageInfo(*img));
  return *img;
}

QString ImageFactory::addImage(const QImage& image_, const QString& format_) {
  return addImageImpl(image_, format_).id();
}

QString ImageFactory::addImage(const QPixmap& pix_, const QString& format_) {
  return addImageImpl(pix_.convertToImage(), format_).id();
}

const Tellico::Data::Image& ImageFactory::addImageImpl(const QImage& image_, const QString& format_) {
  Data::Image* img = new Data::Image(image_, format_);
  if(hasImage(img->id())) {
    const Data::Image& img2 = imageById(img->id());
    if(!img2.isNull()) {
      delete img;
      return img2;
    }
  }
  if(img->isNull()) {
    delete img;
    return s_null;
  }
  s_imageDict.insert(img->id(), img);
  s_imageInfoMap.insert(img->id(), Data::ImageInfo(*img));
  return *img;
}

QString ImageFactory::addImage(const QByteArray& data_, const QString& format_, const QString& id_) {
  return addImageImpl(data_, format_, id_).id();
}

const Tellico::Data::Image& ImageFactory::addImageImpl(const QByteArray& data_, const QString& format_,
                                                       const QString& id_) {
  if(id_.isEmpty()) {
    return s_null;
  }

  // do not call imageById(), it causes infinite looping with Document::loadImage()
  Data::Image* img = s_imageCache.find(id_);
  if(img) {
    myLog() << "ImageFactory::addImageImpl(QByteArray) - already exists in cache: " << id_ << endl;
    return *img;
  }

  img = s_imageDict.find(id_);
  if(img) {
    myLog() << "ImageFactory::addImageImpl(QByteArray) - already exists in dict: " << id_ << endl;
    return *img;
  }

  img = new Data::Image(data_, format_, id_);
  if(img->isNull()) {
    myDebug() << "ImageFactory::addImageImpl(QByteArray) - NULL IMAGE!!!!!" << endl;
    delete img;
    return s_null;
  }

//  myLog() << "ImageFactory::addImageImpl(QByteArray) - " << data_.size()
//          << " bytes, format = " << format_
//          << ", id = "<< img->id() << endl;

  s_imageDict.insert(img->id(), img);
  s_imageInfoMap.insert(img->id(), Data::ImageInfo(*img));
  return *img;
}

const Tellico::Data::Image& ImageFactory::addCachedImageImpl(const QString& id_, CacheDir dir_) {
//  myLog() << "ImageFactory::addCachedImageImpl() - dir = " << (dir_ == DataDir ? "DataDir" : "TmpDir" )
//                                                           << "; id = " << id_ << endl;
  KURL u;
  if(dir_ == DataDir) {
    u.setPath(dataDir() + id_);
  } else if(dir_ == LocalDir) {
    u.setPath(localDir() + id_);
  } else{ // Temp
    u.setPath(tempDir() + id_);
  }

  QString newID = addImage(u, true);
  if(newID.isEmpty()) {
    myLog() << "ImageFactory::addCachedImageImpl() - null image loaded" << endl;
    return s_null;
  }

  // the id probably got changed, so reset it
  // addImage() already inserted it in the dict
  Data::Image* img = s_imageDict.take(newID);
  if(!img) {
    kdWarning() << "ImageFactory::addCachedImageImpl() - no image in dict - very bad!" << endl;
    return s_null;
  }
  if(img->isNull()) {
    kdWarning() << "ImageFactory::addCachedImageImpl() - null image in dict, should never happen!" << endl;
    delete img;
    return s_null;
  }
  img->setID(id_);
  s_imageInfoMap.remove(newID);
  s_imageInfoMap.insert(img->id(), Data::ImageInfo(*img));

  if(s_imageCache.insert(img->id(), img, img->numBytes())) {
//    myLog() << "ImageFactory::addCachedImageImpl() - removing from dict: " << img->id() << endl;
  } else {
    // can't hold it in the cache
    kdWarning() << "Tellico's image cache is unable to hold the image, it might be too big!" << endl;
    kdWarning() << "Image name is " << img->id() << endl;
    kdWarning() << "Image size is " << img->numBytes() << endl;
    kdWarning() << "Max cache size is " << s_imageCache.maxCost() << endl;

    // add it back to the dict, but add the image to the list of
    // images to release later. Necessary to avoid a memory leak since new Image()
    // was called, we need to keep the pointer
    s_imageDict.insert(img->id(), img);
    s_imagesToRelease.add(img->id());
  }
  return *img;
}

bool ImageFactory::writeImage(const QString& id_, const KURL& targetDir_, bool force_) {
//  myLog() << "ImageFactory::writeImage() - target = " << targetDir_.url() << id_ << endl;
  if(targetDir_.isEmpty()) {
    myDebug() << "ImageFactory::writeImage() - empty target dir!" << endl;
    return false;
  }

  const Data::Image& img = imageById(id_);
  if(img.isNull()) {
//    myDebug() << "ImageFactory::writeImage() - null image: " << id_ << endl;
    return false;
  }

  if(img.linkOnly()) {
    myDebug() << "ImageFactory::writeImage() - " << id_ << ": link only, not writing!" << endl;
    return true;
  }

  KURL target = targetDir_;
  target.addPath(id_);

  return FileHandler::writeDataURL(target, img.byteArray(), force_);
}

bool ImageFactory::writeCachedImage(const QString& id_, CacheDir dir_, bool force_ /*=false*/) {
  if(id_.isEmpty()) {
    return false;
  }
//  myLog() << "ImageFactory::writeCachedImage() - dir = " << (dir_ == DataDir ? "DataDir" : "TmpDir" )
//                                                         << "; id = " << id_ << endl;

  QString path = ( dir_ == DataDir ? dataDir() : dir_ == TempDir ? tempDir() : localDir() );

  // images in the temp directory are erased every session, so we can track
  // whether they've already been written with a simple string set.
  // images in the data directory are persistent, so we have to check the
  // actual file existence
  bool exists = ( dir_ == TempDir ? s_imagesInTmpDir.has(id_) : QFile::exists(path + id_));

  if(!force_ && exists) {
//    myDebug() << "...writeCachedImage() - exists = true: " << id_ << endl;
  } else if(!force_ && !exists && dir_ == LocalDir) {
    QDir dir(localDir());
    if(!dir.exists()) {
      myDebug() << "ImageFactory::writeCachedImage() - creating " << s_localDir << endl;
      dir.mkdir(localDir());
    }
  } else {
//    myLog() << "ImageFactory::writeCachedImage() - dir = " << (dir_ == DataDir ? "DataDir" : "TmpDir" )
//                                                           << "; id = " << id_ << endl;
  }
  // only write if it doesn't exist
  bool success = (!force_ && exists) || writeImage(id_, path, true /* force */);

  if(success) {
    if(dir_ == TempDir) {
      s_imagesInTmpDir.add(id_);
    }

    // remove from dict and add to cache
    // it might not be in dict though
    Data::Image* img = s_imageDict.take(id_);
    if(img && s_imageCache.insert(img->id(), img, img->numBytes())) {
      s_imageInfoMap.remove(id_);
    } else if(img) {
//      myLog() << "ImageFactory::writeCachedImage() - failed writing image to cache: " << id_ << endl;
//      myLog() << "ImageFactory::writeCachedImage() - removed from dict, deleting: " << img->id() << endl;
//      myLog() << "ImageFactory::writeCachedImage() - current dict size: " << s_imageDict.count() << endl;
      // can't insert it in the cache, so put it back in the dict
      // No, it's written to disk now, so we're safe
//      s_imageDict.insert(img->id(), img);
      delete img;
    }
  }
  return success;
}

const Tellico::Data::Image& ImageFactory::imageById(const QString& id_) {
  if(id_.isEmpty()) {
    myDebug() << "ImageFactory::imageById() - empty id" << endl;
    return s_null;
  }
//  myLog() << "ImageFactory::imageById() - " << id_ << endl;

  // can't think of a better place to regularly check for images to release
  // but don't release image that just got asked for
  s_imagesToRelease.remove(id_);
  releaseImages();

 // first check the cache, used for images that are in the data file, or are only temporary
 // then the dict, used for images downloaded, but not yet saved anywhere
  Data::Image* img = s_imageCache.find(id_);
  if(img) {
//    myLog() << "...imageById() - found in cache" << endl;
    return *img;
  }

  img = s_imageDict.find(id_);
  if(img) {
//    myLog() << "...imageById() - found in dict" << endl;
    return *img;
  }

  // if the image is link only, we need to load it
  // but can't call imageInfo() since that might recurse into imageById()
  // also, the image info cache might not have it so check if the
  // id is a valid absolute url
  // yeah, it's probably slow
  if((s_imageInfoMap.contains(id_) && s_imageInfoMap[id_].linkOnly) || !KURL::isRelativeURL(id_)) {
    KURL u = id_;
    if(u.isValid()) {
      return addImageImpl(u, false, KURL(), true);
    }
  }

  // the document does a delayed loading of the images, sometimes
  // so an image could be in the tmp dir and not be in the cache
  // or it could be too big for the cache
  if(s_imagesInTmpDir.has(id_)) {
    const Data::Image& img2 = addCachedImageImpl(id_, TempDir);
    if(!img2.isNull()) {
//      myLog() << "...imageById() - found in tmp dir" << endl;
      return img2;
    } else {
      myLog() << "ImageFactory::imageById() - img in tmpDir list but not actually there: " << id_ << endl;
      s_imagesInTmpDir.remove(id_);
    }
  }

  // try to do a delayed loading of the image
  if(Data::Document::self()->loadImage(id_)) {
    // loadImage() could insert in either the cache or the dict!
    img = s_imageCache.find(id_);
    if(!img) {
      img = s_imageDict.find(id_);
    }
    if(img) {
//      myLog() << "...imageById() - found in doc" << endl;
      // go ahead and write image to disk so we don't have to keep it in memory
      // calling pixmap() could be loading all the covers, and we don't want one
      // to get pushed out of the cache yet
      if(!s_imagesInTmpDir.has(id_)) {
        writeCachedImage(id_, TempDir);
      }
      return *img;
    }
  }

  // don't check Config::writeImagesInFile(), someday we might have problems
  // and the image will exist in the data dir, but the app thinks everything should
  // be in the zip file instead
  bool exists = QFile::exists(dataDir() + id_);
  if(exists) {
    // if we're loading from the application data dir, but images are being saved in the
    // data file instead, then consider the document to be modified since it needs
    // the image saved
    if(Config::imageLocation() != Config::ImagesInAppDir) {
      Data::Document::self()->slotSetModified(true);
    }
    const Data::Image& img2 = addCachedImageImpl(id_, DataDir);
    if(img2.isNull()) {
      myDebug() << "ImageFactory::imageById() - tried to add from DataDir, but failed: " << id_ << endl;
    } else {
//      myLog() << "...imageById() - found in data dir" << endl;
      return img2;
    }
  }
  // if localDir() == DataDir(), then there's nothing left to check
  if(localDir() == dataDir()) {
    return s_null;
  }
  exists = QFile::exists(localDir() + id_);
  if(exists) {
    // if we're loading from the application data dir, but images are being saved in the
    // data file instead, then consider the document to be modified since it needs
    // the image saved
    if(Config::imageLocation() != Config::ImagesInLocalDir) {
      Data::Document::self()->slotSetModified(true);
    }
    const Data::Image& img2 = addCachedImageImpl(id_, LocalDir);
    if(img2.isNull()) {
      myDebug() << "ImageFactory::imageById() - tried to add from LocalDir, but failed: " << id_ << endl;
    } else {
//      myLog() << "...imageById() - found in data dir" << endl;
      return img2;
    }
  }
  myDebug() << "***ImageFactory::imageById() - not found: " << id_ << endl;
  return s_null;
}

Tellico::Data::ImageInfo ImageFactory::imageInfo(const QString& id_) {
  if(s_imageInfoMap.contains(id_)) {
    return s_imageInfoMap[id_];
  }

  const Data::Image& img = imageById(id_);
  if(img.isNull()) {
    return Data::ImageInfo();
  }
  return Data::ImageInfo(img);
}

void ImageFactory::cacheImageInfo(const Data::ImageInfo& info) {
  s_imageInfoMap.insert(info.id, info);
}

bool ImageFactory::validImage(const QString& id_) {
  // don't try s_imageInfoMap[id_] cause it inserts an empty image info
  return s_imageInfoMap.contains(id_) || hasImage(id_) || !imageById(id_).isNull();
}

QPixmap ImageFactory::pixmap(const QString& id_, int width_, int height_) {
  if(id_.isEmpty()) {
    return QPixmap();
  }

  const QString key = id_ + '|' + QString::number(width_) + '|' + QString::number(height_);
  QPixmap* pix = s_pixmapCache.find(key);
  if(pix) {
    return *pix;
  }

  const Data::Image& img = imageById(id_);
  if(img.isNull()) {
    return QPixmap();
  }

  if(width_ > 0 && height_ > 0) {
    pix = new QPixmap(img.convertToPixmap(width_, height_));
  } else {
    pix = new QPixmap(img.convertToPixmap());
  }

  // pixmap size is w x h x d, divided by 8 bits
  if(!s_pixmapCache.insert(key, pix, pix->width()*pix->height()*pix->depth()/8)) {
    kdWarning() << "ImageFactory::pixmap() - can't save in cache: " << id_ << endl;
    kdWarning() << "### Current pixmap size is " << (pix->width()*pix->height()*pix->depth()/8) << endl;
    kdWarning() << "### Max pixmap cache size is " << s_pixmapCache.maxCost() << endl;
    QPixmap pix2(*pix);
    delete pix;
    return pix2;
  }
  return *pix;
}

void ImageFactory::clean(bool deleteTempDirectory_) {
  // the dict and caches all auto-delete
  s_imagesToRelease.clear();
  s_imageDict.clear();
  s_imageInfoMap.clear();
  s_imageCache.clear();
  s_pixmapCache.clear();
  if(deleteTempDirectory_) {
    s_imagesInTmpDir.clear();
    delete s_tmpDir;
    s_tmpDir = 0;
  }
}

void ImageFactory::createStyleImages(const StyleOptions& opt_) {
  const int collType = Kernel::self()->collectionType();

  const QColor& baseColor = opt_.baseColor.isValid()
                          ? opt_.baseColor
                          : Config::templateBaseColor(collType);
  const QColor& highColor = opt_.highlightedBaseColor.isValid()
                          ? opt_.highlightedBaseColor
                          : Config::templateHighlightedBaseColor(collType);

  const QColor& bgc1 = Tellico::blendColors(baseColor, highColor, 30);
  const QColor& bgc2 = Tellico::blendColors(baseColor, highColor, 50);

  const QString bgname = QString::fromLatin1("gradient_bg.png");
  QImage bgImage = KImageEffect::gradient(QSize(400, 1), bgc1, baseColor,
                                          KImageEffect::PipeCrossGradient);
  bgImage = KImageEffect::rotate(bgImage, KImageEffect::Rotate90);

  const QString hdrname = QString::fromLatin1("gradient_header.png");
  QImage hdrImage = KImageEffect::unbalancedGradient(QSize(1, 10), highColor, bgc2,
                                                     KImageEffect::VerticalGradient, 100, -100);

  if(opt_.imgDir.isEmpty()) {
    // write the style images both to the tmp dir and the data dir
    // doesn't really hurt and lets the user switch back and forth
    ImageFactory::removeImage(bgname, true /*delete */);
    ImageFactory::addImageImpl(Data::Image::byteArray(bgImage, "PNG"), QString::fromLatin1("PNG"), bgname);
    ImageFactory::writeCachedImage(bgname, DataDir, true /*force*/);
    ImageFactory::writeCachedImage(bgname, TempDir, true /*force*/);

    ImageFactory::removeImage(hdrname, true /*delete */);
    ImageFactory::addImageImpl(Data::Image::byteArray(hdrImage, "PNG"), QString::fromLatin1("PNG"), hdrname);
    ImageFactory::writeCachedImage(hdrname, DataDir, true /*force*/);
    ImageFactory::writeCachedImage(hdrname, TempDir, true /*force*/);
  } else {
    bgImage.save(opt_.imgDir + bgname, "PNG");
    hdrImage.save(opt_.imgDir + hdrname, "PNG");
  }
}

void ImageFactory::removeImage(const QString& id_, bool deleteImage_) {
  //be careful using this
  s_imageDict.remove(id_);
  s_imageCache.remove(id_);
  s_imagesInTmpDir.remove(id_);

  if(deleteImage_) {
    // remove from both data dir and temp dir
    QFile::remove(dataDir() + id_);
    QFile::remove(tempDir() + id_);
  }
}

Tellico::StringSet ImageFactory::imagesNotInCache() {
  StringSet set;
  for(QDictIterator<Tellico::Data::Image> it(s_imageDict); it.current(); ++it) {
    if(s_imageCache.find(it.currentKey()) == 0) {
      set.add(it.currentKey());
    }
  }
  return set;
}

bool ImageFactory::hasImage(const QString& id_) {
  return s_imageCache.find(id_, false) || s_imageDict.find(id_);
}

// the purpose here is to remove images from the dict if they're is on the disk somewhere,
// either in tempDir() or in dataDir(). The use for this is for calling pixmap() on an
// image too big to stay in the cache. Then it stays in the dict forever.
void ImageFactory::releaseImages() {
#ifdef RELEASE_IMAGES
  if(s_imagesToRelease.isEmpty()) {
    return;
  }

  const QStringList images = s_imagesToRelease.toList();
  for(QStringList::ConstIterator it = images.begin(); it != images.end(); ++it) {
    s_imagesToRelease.remove(*it);
    if(!s_imageDict.find(*it)) {
      continue;
    }
//    myLog() << "ImageFactory::releaseImage() - id = " << *it << endl;
    if(QFile::exists(dataDir() + *it)) {
//      myDebug() << "...exists in dataDir() - removing from dict" << endl;
      s_imageDict.remove(*it);
    } else if(QFile::exists(tempDir() + *it)) {
//      myDebug() << "...exists in tempDir() - removing from dict" << endl;
      s_imageDict.remove(*it);
    }
  }
#endif
}

void ImageFactory::setLocalDirectory(const KURL& url_) {
  if(url_.isEmpty()) {
    return;
  }
  if(!url_.isLocalFile()) {
    myWarning() << "ImageFactory::setLocalDirectory() - Tellico can only save images to local disk" << endl;
    myWarning() << "unable to save to " << url_ << endl;
  } else {
    s_localDir = url_.directory(false);
    // could have already been set once
    if(!url_.fileName().contains(QString::fromLatin1("_files"))) {
      s_localDir += url_.fileName().section('.', 0, 0) + QString::fromLatin1("_files/");
    }
    myLog() << "ImageFactory::setLocalDirectory() - local dir = " << s_localDir << endl;
  }
}

#undef RELEASE_IMAGES

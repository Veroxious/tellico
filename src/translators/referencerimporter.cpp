/***************************************************************************
    copyright            : (C) 2007-2008 by Robby Stephenson
    email                : robby@periapsis.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of version 2 of the GNU General Public License as  *
 *   published by the Free Software Foundation;                            *
 *                                                                         *
 ***************************************************************************/

#include "referencerimporter.h"
#include "../collection.h"
#include "../core/netaccess.h"
#include "../imagefactory.h"

#include <kstandarddirs.h>
#include <kdebug.h>

#include <QPixmap>

using Tellico::Import::ReferencerImporter;

ReferencerImporter::ReferencerImporter(const KUrl& url_) : XSLTImporter(url_) {
  QString xsltFile = KStandardDirs::locate("appdata", QLatin1String("referencer2tellico.xsl"));
  if(!xsltFile.isEmpty()) {
    KUrl u;
    u.setPath(xsltFile);
    XSLTImporter::setXSLTURL(u);
  } else {
    kWarning() << "ReferencerImporter() - unable to find referencer2tellico.xml!";
  }
}

bool ReferencerImporter::canImport(int type) const {
  return type == Data::Collection::Bibtex;
}

Tellico::Data::CollPtr ReferencerImporter::collection() {
  Data::CollPtr coll = XSLTImporter::collection();
  if(!coll) {
    return Data::CollPtr();
  }

  Data::FieldPtr field = coll->fieldByName(QLatin1String("cover"));
  if(!field && !coll->imageFields().isEmpty()) {
    field = coll->imageFields().front();
  } else if(!field) {
    field = new Data::Field(QLatin1String("cover"), i18n("Front Cover"), Data::Field::Image);
    coll->addField(field);
  }

  foreach(Data::EntryPtr entry, coll->entries()) {
    QString url = entry->field(QLatin1String("url"));
    if(url.isEmpty()) {
      continue;
    }
    QPixmap pix = NetAccess::filePreview(url);
    if(pix.isNull()) {
      continue;
    }
    QString id = ImageFactory::addImage(pix, QLatin1String("PNG"));
    if(id.isEmpty()) {
      continue;
    }
    entry->setField(field, id);
  }
  return coll;
}

#include "referencerimporter.moc"

/***************************************************************************
    copyright            : (C) 2004-2007 by Robby Stephenson
    email                : robby@periapsis.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of version 2 of the GNU General Public License as  *
 *   published by the Free Software Foundation;                            *
 *                                                                         *
 ***************************************************************************/

#ifndef AUDIOFILEIMPORTER_H
#define AUDIOFILEIMPORTER_H

class QCheckBox;

#include "importer.h"
#include "../datavectors.h"

namespace TagLib {
  class FileRef;
}

namespace Tellico {
  namespace Import {

/**
 * The AudioFileImporter class takes care of importing audio files.
 *
 * @author Robby Stephenson
 */
class AudioFileImporter : public Importer {
Q_OBJECT

public:
  /**
   */
  AudioFileImporter(const KURL& url);

  /**
   */
  virtual Data::CollPtr collection();
  /**
   */
  virtual QWidget* widget(QWidget* parent, const char* name=0);
  virtual bool canImport(int type) const;

public slots:
  void slotCancel();
  void slotAddFileToggled(bool on);

private:
  static QString insertValue(const QString& str, const QString& value, uint pos);

  int discNumber(const TagLib::FileRef& file) const;

  Data::CollPtr m_coll;
  QWidget* m_widget;
  QCheckBox* m_recursive;
  QCheckBox* m_addFilePath;
  QCheckBox* m_addBitrate;
  bool m_cancelled : 1;
};

  } // end namespace
} // end namespace
#endif

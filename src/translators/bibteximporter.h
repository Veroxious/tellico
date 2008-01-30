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

#ifndef BIBTEXIMPORTER_H
#define BIBTEXIMPORTER_H

#include <config.h>
#include "importer.h"
#include "../datavectors.h"

extern "C" {
#ifdef HAVE_LIBBTPARSE
#include <btparse.h>
#else
#include "btparse/btparse.h"
}
#endif

#include <qptrlist.h>
#include <qmap.h>

class QRadioButton;

namespace Tellico {
  namespace Import {

/**
 * Bibtex files are used for bibliographies within LaTex. The btparse library is used to
 * parse the text and generate a @ref BibtexCollection.
 *
 * @author Robby Stephenson
 */
class BibtexImporter : public Importer {
Q_OBJECT

public:
  /**
   * Initializes the btparse library
   *
   * @param url The url of the bibtex file
   */
  BibtexImporter(const KURL::List& urls);
  BibtexImporter(const QString& text);
  /*
   * Some cleanup is done for the btparse library
   */
  virtual ~BibtexImporter();

  /**
   * Returns a pointer to a @ref BibtexCollection created on the stack. All entries
   * in the bibtex file are added, including any preamble, all macro strings, and each entry.
   *
   * @return A pointer to a @ref BibtexCollection, or 0 if none can be created.
   */
  virtual Data::CollPtr collection();
  virtual QWidget* widget(QWidget* parent, const char* name=0);
  virtual bool canImport(int type) const;

public slots:
  void slotCancel();

private:
  Data::CollPtr readCollection(const QString& text, int n);
  void parseText(const QString& text);

  typedef QPtrList<AST> ASTList;
  typedef QPtrListIterator<AST> ASTListIterator;
  ASTList m_nodes;
  QMap<QString, QString> m_macros;

  Data::CollPtr m_coll;
  QWidget* m_widget;
  QRadioButton* m_readUTF8;
  QRadioButton* m_readLocale;
  bool m_cancelled : 1;
};

  } // end namespace
} // end namespace
#endif

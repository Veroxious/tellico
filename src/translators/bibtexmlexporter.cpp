/*************************************************************************
    copyright            : (C) 2003-2004 by Robby Stephenson
    email                : robby@periapsis.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of version 2 of the GNU General Public License as  *
 *   published by the Free Software Foundation;                            *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "bibtexmlexporter.h"
#include "bibtexhandler.h"
#include "../collection.h"
#include "../collections/bibtexcollection.h"
#include "../latin1literal.h"
#include "tellico_xml.h"

#include <klocale.h>
#include <kdebug.h>

#include <qvbox.h>
#include <qdom.h>
#include <qregexp.h>
#include <qtextcodec.h>

using Tellico::Export::BibtexmlExporter;

QString BibtexmlExporter::formatString() const {
  return i18n("Bibtexml");
}

QString BibtexmlExporter::fileFilter() const {
  return i18n("*.xml|Bibtexml files (*.xml)") + QChar('\n') + i18n("*|All files");
}

QString BibtexmlExporter::text(bool formatFields_, bool encodeUTF8_) {
  if(collection()->type() != Data::Collection::Bibtex) {
    return QString::null;
  }

// there are some special fields
// the entry-type specifies the entry type - book, inproceedings, whatever
  QString typeField;
// the key specifies the cite-key
  QString keyField;
// keep a list of all the 'ordinary' fields to iterate through later
  Data::FieldList fieldList;
  for(Data::FieldListIterator it(collection()->fieldList()); it.current(); ++it) {
    QString bibtex = it.current()->property(QString::fromLatin1("bibtex"));
    if(bibtex == Latin1Literal("entry-type")) {
      typeField = it.current()->name();
    } else if(bibtex == Latin1Literal("key")) {
      keyField = it.current()->name();
    } else if(!bibtex.isEmpty()) {
      fieldList.append(it.current());
    }
  }

  QDomImplementation impl;
  QDomDocumentType doctype = impl.createDocumentType(QString::fromLatin1("file"),
                                                     QString::null,
                                                     XML::dtdBibtexml);
  //default namespace
  const QString& ns = XML::nsBibtexml;

  QDomDocument dom = impl.createDocument(ns, QString::fromLatin1("file"), doctype);

  // root element
  QDomElement root = dom.documentElement();

  QString encodeStr = QString::fromLatin1("version=\"1.0\" encoding=\"");
  if(encodeUTF8_) {
    encodeStr += QString::fromLatin1("UTF-8");
  } else {
    encodeStr += QString::fromLatin1(QTextCodec::codecForLocale()->mimeName());
  }
  encodeStr += QString::fromLatin1("\"");

  // createDocument creates a root node, insert the processing instruction before it
  dom.insertBefore(dom.createProcessingInstruction(QString::fromLatin1("xml"), encodeStr), root);
  QString comment = QString::fromLatin1("Generated by Tellico ") + QString::fromLatin1(VERSION);
  dom.insertBefore(dom.createComment(comment), root);

  QStringList macros = static_cast<const Data::BibtexCollection*>(collection())->macroList().keys();

  QString entryName, key, value;
  QDomElement btElem, entryElem, ParentElem, fieldElem;
  Data::FieldListIterator fieldIt(fieldList);
  for(Data::EntryListIterator entryIt(entryList()); entryIt.current(); ++entryIt) {
    key = entryIt.current()->field(keyField);
    if(key.isEmpty()) {
      key = BibtexHandler::bibtexKey(entryIt.current());
    }
    btElem = dom.createElement(QString::fromLatin1("entry"));
    btElem.setAttribute(QString::fromLatin1("id"), key);
    root.appendChild(btElem);

    entryName = entryIt.current()->field(typeField);
    if(entryName.isEmpty()) {
      kdWarning() << "BibtexmlExporter::text() - the entry for '" << entryIt.current()->title()
                  << "' has no entry-type, skipping it!" << endl;
      continue;
    }

    entryElem = dom.createElement(entryName);
    btElem.appendChild(entryElem);

    // now iterate over attributes
    for(fieldIt.toFirst(); fieldIt.current(); ++fieldIt) {
      // get field value
      if(formatFields_) {
        value = entryIt.current()->formattedField(fieldIt.current()->name());
      } else {
        value = entryIt.current()->field(fieldIt.current()->name());
      }
      if(value.isEmpty()) {
        continue;
      }

/* Bibtexml has special container elements for titles, authors, editors, and keywords
   I'm going to ignore the titlelist element for right now. All authors are contained in
   an authorlist element, editors in an editorlist element, and keywords are in a
   keywords element, and themselves as a keyword. Also, Bibtexml can format names
   similar to docbook, with first, middle, last, etc elements. I'm going to ignore that
   for now, too.*/
      QString elemName = fieldIt.current()->property(QString::fromLatin1("bibtex"));
      // split text for author, editor, and keywords
      if(fieldIt.current()->flags() & Data::Field::AllowMultiple
         && (elemName == Latin1Literal("author")
             || elemName == Latin1Literal("editor")
             || elemName == Latin1Literal("keywords"))) {
        QString parElemName;
        if(elemName == Latin1Literal("author")) {
          parElemName = QString::fromLatin1("authorlist");
        } else if(elemName == Latin1Literal("editor")) {
          parElemName = QString::fromLatin1("editorlist");
        } else { // keywords
          parElemName = QString::fromLatin1("keywords");
          elemName = QString::fromLatin1("keyword");
        }

        ParentElem = dom.createElement(parElemName);
        QStringList values = Data::Field::split(value, false);
        for(QStringList::Iterator it = values.begin(); it != values.end(); ++it) {
          fieldElem = dom.createElement(elemName);
          fieldElem.appendChild(dom.createTextNode(*it));
          ParentElem.appendChild(fieldElem);
        }
        entryElem.appendChild(ParentElem);
      } else {
        fieldElem = dom.createElement(elemName);
        fieldElem.appendChild(dom.createTextNode(value));
        entryElem.appendChild(fieldElem);
      }
    }
  }

  return dom.toString();
}

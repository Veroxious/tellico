/***************************************************************************
    copyright            : (C) 2007 by Robby Stephenson
    email                : robby@periapsis.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of version 2 of the GNU General Public License as  *
 *   published by the Free Software Foundation;                            *
 *                                                                         *
 ***************************************************************************/

#include "xmphandler.h"
#include "tellico_debug.h"
#include <config.h>

#include <qfile.h>
#include <qtextstream.h>
#ifdef HAVE_EXEMPI
#include <exempi/xmp.h>
#endif

using Tellico::XMPHandler;

int XMPHandler::s_initCount = 0;

bool XMPHandler::isXMPEnabled() {
#ifdef HAVE_EXEMPI
  return true;
#else
  return false;
#endif
}

XMPHandler::XMPHandler() {
  init();
}

XMPHandler::~XMPHandler() {
#ifdef HAVE_EXEMPI
  --s_initCount;
  if(s_initCount == 0) {
    xmp_terminate();
  }
#endif
}

void XMPHandler::init() {
#ifdef HAVE_EXEMPI
  if(s_initCount == 0) {
    xmp_init();
  }
  ++s_initCount;
#endif
}

QString XMPHandler::extractXMP(const QString& file) {
  QString result;
#ifdef HAVE_EXEMPI
  XmpFilePtr xmpfile = xmp_files_open_new(QFile::encodeName(file), XMP_OPEN_READ);
  if(!xmpfile) {
    myDebug() << "XMPHandler::parse() - unable to open " << file << endl;
    return result;
  }
  XmpPtr xmp = xmp_files_get_new_xmp(xmpfile);
  if(xmp) {
    XmpStringPtr buffer = xmp_string_new();
    xmp_serialize(xmp, buffer, 0, 0);
    result = QString::fromUtf8(xmp_string_cstr(buffer));
    xmp_string_free(buffer);
//    myDebug() << result << endl;
#if 0
    kdWarning() << "XMPHandler::parse() - turn me off!" << endl;
    QFile f1(QString::fromLatin1("/tmp/xmp.xml"));
    if(f1.open(IO_WriteOnly)) {
      QTextStream t(&f1);
      t << result;
    }
    f1.close();
#endif
    xmp_free(xmp);
    xmp_files_close(xmpfile, XMP_CLOSE_NOOPTION);
    xmp_files_free(xmpfile);
  } else {
    myDebug() << "XMPHandler::parse() - unable to parse " << file << endl;
  }
#endif
  return result;
}

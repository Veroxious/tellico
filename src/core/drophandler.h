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

#ifndef TELLICO_DROPHANDLER_H
#define TELLICO_DROPHANDLER_H

#include <qobject.h>
#include <kurl.h>

namespace Tellico {

class DropHandler : public QObject {
Q_OBJECT

public:
  DropHandler(QObject* parent);
  ~DropHandler();

protected:
  bool eventFilter(QObject* object, QEvent* event);

private:
  bool dragEnter(QDragEnterEvent* event);
  bool drop(QDropEvent* event);
  bool handleURL(const KURL::List& urls);
};

}
#endif

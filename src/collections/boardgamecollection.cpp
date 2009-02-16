/***************************************************************************
    copyright            : (C) 2005-2006 by Robby Stephenson, Steve Beattie
    email                : robby@periapsis.org, sbeattie@suse.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of version 2 of the GNU General Public License as  *
 *   published by the Free Software Foundation;                            *
 *                                                                         *
 ***************************************************************************/

#include "boardgamecollection.h"

#include <klocale.h>

namespace {
  static const char* boardgame_general = I18N_NOOP("General");
  static const char* boardgame_personal = I18N_NOOP("Personal");
}

using Tellico::Data::BoardGameCollection;

BoardGameCollection::BoardGameCollection(bool addFields_, const QString& title_ /*=null*/)
   : Collection(title_.isEmpty() ? i18n("My Board Games") : title_) {
  if(addFields_) {
    addFields(defaultFields());
  }
  setDefaultGroupField(QLatin1String("genre"));
}

Tellico::Data::FieldList BoardGameCollection::defaultFields() {
  FieldList list;
  FieldPtr field;

  field = new Field(QLatin1String("title"), i18n("Title"));
  field->setCategory(i18n(boardgame_general));
  field->setFlags(Field::NoDelete);
  field->setFormatFlag(Field::FormatTitle);
  list.append(field);

  field = new Field(QLatin1String("genre"), i18n("Genre"));
  field->setCategory(i18n(boardgame_general));
  field->setFlags(Field::AllowCompletion | Field::AllowMultiple | Field::AllowGrouped);
  field->setFormatFlag(Field::FormatPlain);
  list.append(field);

  field = new Field(QLatin1String("mechanism"), i18n("Mechanism"));
  field->setCategory(i18n(boardgame_general));
  field->setFlags(Field::AllowCompletion | Field::AllowMultiple | Field::AllowGrouped);
  field->setFormatFlag(Field::FormatPlain);
  list.append(field);

  field = new Field(QLatin1String("year"), i18n("Release Year"), Field::Number);
  field->setCategory(i18n(boardgame_general));
  field->setFlags(Field::AllowGrouped);
  list.append(field);

  field = new Field(QLatin1String("publisher"), i18n("Publisher"));
  field->setCategory(i18n(boardgame_general));
  field->setFlags(Field::AllowCompletion | Field::AllowMultiple | Field::AllowGrouped);
  field->setFormatFlag(Field::FormatPlain);
  list.append(field);

  field = new Field(QLatin1String("designer"), i18n("Designer"));
  field->setCategory(i18n(boardgame_general));
  field->setFlags(Field::AllowCompletion | Field::AllowMultiple | Field::AllowGrouped);
  field->setFormatFlag(Field::FormatPlain);
  list.append(field);

  field = new Field(QLatin1String("num-player"), i18n("Number of Players"), Field::Number);
  field->setCategory(i18n(boardgame_general));
  field->setFlags(Field::AllowMultiple | Field::AllowGrouped);
  list.append(field);

  field = new Field(QLatin1String("description"), i18n("Description"), Field::Para);
  list.append(field);

  field = new Field(QLatin1String("rating"), i18n("Rating"), Field::Rating);
  field->setCategory(i18n(boardgame_personal));
  field->setFlags(Field::AllowGrouped);
  list.append(field);

  field = new Field(QLatin1String("pur_date"), i18n("Purchase Date"));
  field->setCategory(i18n(boardgame_personal));
  field->setFormatFlag(Field::FormatDate);
  list.append(field);

  field = new Field(QLatin1String("gift"), i18n("Gift"), Field::Bool);
  field->setCategory(i18n(boardgame_personal));
  list.append(field);

  field = new Field(QLatin1String("pur_price"), i18n("Purchase Price"));
  field->setCategory(i18n(boardgame_personal));
  list.append(field);

  field = new Field(QLatin1String("loaned"), i18n("Loaned"), Field::Bool);
  field->setCategory(i18n(boardgame_personal));
  list.append(field);

  field = new Field(QLatin1String("cover"), i18n("Cover"), Field::Image);
  list.append(field);

  field = new Field(QLatin1String("comments"), i18n("Comments"), Field::Para);
  field->setCategory(i18n(boardgame_personal));
  list.append(field);

  return list;
}

#include "boardgamecollection.moc"

/***************************************************************************
    Copyright (C) 2009 Robby Stephenson <robby@periapsis.org>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of        *
 *   the License or (at your option) version 3 or any later version        *
 *   accepted by the membership of KDE e.V. (or its successor approved     *
 *   by the membership of KDE e.V.), which shall act as a proxy            *
 *   defined in Section 14 of version 3 of the license.                    *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 ***************************************************************************/

#undef QT_NO_CAST_FROM_ASCII

#include "entitytest.h"

#include "../utils/string_utils.h"

#include <QTest>

QTEST_APPLESS_MAIN( EntityTest )

#define QSL(x) QStringLiteral(x)

void EntityTest::testEntities() {
  QFETCH(QByteArray, data);
  QFETCH(QString, expectedString);

  QCOMPARE(Tellico::decodeHTML(data), expectedString);
}

void EntityTest::testEntities_data() {
  QTest::addColumn<QByteArray>("data");
  QTest::addColumn<QString>("expectedString");

  QTest::newRow("robby") << QByteArray("robby") << QSL("robby");
  QTest::newRow("&fake;") << QByteArray("&fake;") << QSL("&fake;");
  QTest::newRow("&#48;") << QByteArray("&#48;") << QSL("0");
  QTest::newRow("robby&#48;robb") << QByteArray("robby&#48;robby") << QSL("robby0robby");
}

void EntityTest::testAccents() {
  QFETCH(QString, inputString);
  QFETCH(QString, expectedString);

  QCOMPARE(Tellico::removeAccents(inputString), expectedString);
}

void EntityTest::testAccents_data() {
  QTest::addColumn<QString>("inputString");
  QTest::addColumn<QString>("expectedString");

  QTest::newRow("robby") << QSL("robby") << QSL("robby");
  QTest::newRow("jose") << QSL("José Guzmán") << QSL("Jose Guzman");
  QTest::newRow("inarritu") << QSL("Alejandro González Iñárritu") << QSL("Alejandro Gonzalez Inarritu");
  QTest::newRow("harakiri") << QSL("'Shitsurakuen': jôbafuku onna harakiri") << QSL("'Shitsurakuen': jobafuku onna harakiri");
  QTest::newRow("svet") << QSL("Tmavomodrý Svět") << QSL("Tmavomodry Svet");
  QTest::newRow("russian") << QSL("Возвращение Супермена") << QSL("Возвращение Супермена");
  QTest::newRow("chinese") << QSL("湖南科学技术出版社") << QSL("湖南科学技术出版社");
}

void EntityTest::testI18nReplace() {
  QFETCH(QString, inputString);
  QFETCH(QString, expectedString);

  QCOMPARE(Tellico::i18nReplace(inputString), expectedString);
}

void EntityTest::testI18nReplace_data() {
  QTest::addColumn<QString>("inputString");
  QTest::addColumn<QString>("expectedString");

  QTest::newRow("robby") << QSL("robby") << QSL("robby");
  QTest::newRow("basic1") << QSL("<i18n>robby</i18n>") << QSL("robby");
  QTest::newRow("basic2") << QSL("<i18n>robby davy</i18n>") << QSL("robby davy");
  QTest::newRow("basic3") << QSL("\n   <i18n>robby</i18n>  \n davy\n") << QSL("robby davy\n");
  // KDE bug 254863
  QTest::newRow("bug254863") << QSL("<i18n>Cer&ca</i18n>") << QSL("Cer&amp;ca");
  QTest::newRow("multiple") << QSL("<i18n>robby</i18n> davy <i18n>jason</i18n>") << QSL("robby davy jason");
  QTest::newRow("bracket") << QSL("<i18n>robby <robby></i18n>") << QSL("robby &lt;robby&gt;");
}

void EntityTest::testMinutes() {
  QFETCH(int, seconds);
  QFETCH(QString, minutesString);

  QCOMPARE(Tellico::minutes(seconds), minutesString);
}

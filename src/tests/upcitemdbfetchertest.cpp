/***************************************************************************
    Copyright (C) 2021 Robby Stephenson <robby@periapsis.org>
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

#include "upcitemdbfetchertest.h"

#include "../fetch/upcitemdbfetcher.h"
#include "../collections/videocollection.h"
#include "../entry.h"
#include "../images/imagefactory.h"

#include <KSharedConfig>
#include <KConfigGroup>

#include <QTest>

QTEST_GUILESS_MAIN( UPCItemDbFetcherTest )

UPCItemDbFetcherTest::UPCItemDbFetcherTest() : AbstractFetcherTest() {
}

void UPCItemDbFetcherTest::initTestCase() {
  Tellico::ImageFactory::init();
}

void UPCItemDbFetcherTest::testFightClub() {
  KConfigGroup cg = KSharedConfig::openConfig(QString(), KConfig::SimpleConfig)->group(QStringLiteral("upcitemdb"));
  cg.writeEntry("Custom Fields", QStringLiteral("barcode"));

  Tellico::Fetch::FetchRequest request(Tellico::Data::Collection::Video, Tellico::Fetch::UPC,
                                       QStringLiteral("024543617907"));
  Tellico::Fetch::Fetcher::Ptr fetcher(new Tellico::Fetch::UPCItemDbFetcher(this));
  fetcher->readConfig(cg);

  Tellico::Data::EntryList results = DO_FETCH1(fetcher, request, 1);

  QCOMPARE(results.size(), 1);

  Tellico::Data::EntryPtr entry = results.at(0);
  QVERIFY(entry);

  QCOMPARE(entry->field(QStringLiteral("title")), QStringLiteral("Fight Club"));
  QCOMPARE(entry->field(QStringLiteral("year")), QStringLiteral("1999"));
  QCOMPARE(entry->field(QStringLiteral("medium")), QStringLiteral("Blu-ray"));
  QCOMPARE(entry->field(QStringLiteral("studio")), QStringLiteral("20th Century Studios"));
  QVERIFY(!entry->field(QStringLiteral("cover")).isEmpty());
  QVERIFY(!entry->field(QStringLiteral("cover")).contains(QLatin1Char('/')));
  QVERIFY(!entry->field(QStringLiteral("plot")).isEmpty());
}
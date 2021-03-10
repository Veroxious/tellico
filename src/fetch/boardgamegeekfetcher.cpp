/***************************************************************************
    Copyright (C) 2014 Robby Stephenson <robby@periapsis.org>
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

#include "boardgamegeekfetcher.h"
#include "../translators/xslthandler.h"
#include "../translators/tellicoimporter.h"
#include "../utils/string_utils.h"
#include "../tellico_debug.h"

#include <KLocalizedString>
#include <KConfigGroup>

#include <QLabel>
#include <QFile>
#include <QTextStream>
#include <QVBoxLayout>
#include <QTextCodec>
#include <QUrlQuery>

namespace {
  // a lot of overlap with boardgamegeekimporter.h
  static const int BGG_MAX_RETURNS_TOTAL = 10;
  static const char* BGG_SEARCH_URL  = "http://boardgamegeek.com/xmlapi2/search";
  static const char* BGG_THING_URL  = "http://boardgamegeek.com/xmlapi2/thing";
}

using Tellico::Fetch::BoardGameGeekFetcher;

BoardGameGeekFetcher::BoardGameGeekFetcher(QObject* parent_)
    : XMLFetcher(parent_) {
  setLimit(BGG_MAX_RETURNS_TOTAL);
  setXSLTFilename(QStringLiteral("boardgamegeek2tellico.xsl"));
}

BoardGameGeekFetcher::~BoardGameGeekFetcher() {
}

QString BoardGameGeekFetcher::source() const {
  return m_name.isEmpty() ? defaultName() : m_name;
}

// https://boardgamegeek.com/wiki/page/XML_API_Terms_of_Use
QString BoardGameGeekFetcher::attribution() const {
  return i18n("This information was freely provided by <a href=\"https://boardgamegeek.com\">BoardGameGeek</a>.");
}

bool BoardGameGeekFetcher::canFetch(int type) const {
  return type == Data::Collection::BoardGame;
}

QUrl BoardGameGeekFetcher::searchUrl() {
  QUrl u(QString::fromLatin1(BGG_SEARCH_URL));

  QUrlQuery q;
  switch(request().key()) {
    case Title:
      q.addQueryItem(QStringLiteral("query"), request().value());
      q.addQueryItem(QStringLiteral("type"), QStringLiteral("boardgame,boardgameexpansion"));
      q.addQueryItem(QStringLiteral("exact"), QStringLiteral("1"));
      break;

    case Keyword:
      q.addQueryItem(QStringLiteral("query"), request().value());
      q.addQueryItem(QStringLiteral("type"), QStringLiteral("boardgame,boardgameexpansion"));
      break;

    case Raw:
      u.setUrl(QLatin1String(BGG_THING_URL));
      q.addQueryItem(QStringLiteral("id"), request().value());
      q.addQueryItem(QStringLiteral("type"), QStringLiteral("boardgame,boardgameexpansion"));
      break;

    default:
      myWarning() << "key not recognized: " << request().key();
      return QUrl();
  }
  u.setQuery(q);

//  myDebug() << "url: " << u.url();
  return u;
}

Tellico::Data::EntryPtr BoardGameGeekFetcher::fetchEntryHookData(Data::EntryPtr entry_) {
  Q_ASSERT(entry_);

  const QString id = entry_->field(QStringLiteral("bggid"));
  if(id.isEmpty()) {
    myDebug() << "no bgg id found";
    return entry_;
  }

  QUrl u(QString::fromLatin1(BGG_THING_URL));
  QUrlQuery q;
  q.addQueryItem(QStringLiteral("id"), id);
  q.addQueryItem(QStringLiteral("type"), QStringLiteral("boardgame,boardgameexpansion"));
  u.setQuery(q);
//  myDebug() << "url: " << u;

  // quiet
  QString output = FileHandler::readXMLFile(u, true);

#if 0
  myWarning() << "Remove output debug from boardgamegeekfetcher.cpp";
  QFile f(QLatin1String("/tmp/test.xml"));
  if(f.open(QIODevice::WriteOnly)) {
    QTextStream t(&f);
    t.setCodec("UTF-8");
    t << output;
  }
  f.close();
#endif

  Import::TellicoImporter imp(xsltHandler()->applyStylesheet(output));
  // be quiet when loading images
  imp.setOptions(imp.options() ^ Import::ImportShowImageErrors);
  Data::CollPtr coll = imp.collection();
//  getTracks(entry);
  if(!coll) {
    myWarning() << "no collection pointer";
    return entry_;
  }
  if(coll->entryCount() == 0) {
    myWarning() << "no entries";
    return entry_;
  }

  if(coll->entryCount() > 1) {
    myDebug() << "weird, more than one entry found";
  }

  // don't want to include id
  coll->removeField(QStringLiteral("bggid"));
  return coll->entries().front();
}

Tellico::Fetch::FetchRequest BoardGameGeekFetcher::updateRequest(Data::EntryPtr entry_) {
  QString bggid = entry_->field(QStringLiteral("bggid"));
  if(!bggid.isEmpty()) {
    return FetchRequest(Raw, bggid);
  }

  QString title = entry_->field(QStringLiteral("title"));
  if(!title.isEmpty()) {
    return FetchRequest(Title, title);
  }
  return FetchRequest();
}

Tellico::Fetch::ConfigWidget* BoardGameGeekFetcher::configWidget(QWidget* parent_) const {
  return new BoardGameGeekFetcher::ConfigWidget(parent_, this);
}

QString BoardGameGeekFetcher::defaultName() {
  return QStringLiteral("BoardGameGeek");
}

QString BoardGameGeekFetcher::defaultIcon() {
  return favIcon("https://cf.geekdo-static.com/icons/favicon2.ico");
}

Tellico::StringHash BoardGameGeekFetcher::allOptionalFields() {
  StringHash hash;
  hash[QStringLiteral("artist")]             = i18nc("Comic Book Illustrator", "Artist");
  hash[QStringLiteral("boardgamegeek-link")] = i18n("BoardGameGeek Link");
  return hash;
}

BoardGameGeekFetcher::ConfigWidget::ConfigWidget(QWidget* parent_, const BoardGameGeekFetcher* fetcher_)
    : Fetch::ConfigWidget(parent_) {
  QVBoxLayout* l = new QVBoxLayout(optionsWidget());
  l->addWidget(new QLabel(i18n("This source has no options."), optionsWidget()));
  l->addStretch();

  // now add additional fields widget
  addFieldsWidget(BoardGameGeekFetcher::allOptionalFields(), fetcher_ ? fetcher_->optionalFields() : QStringList());
}

void BoardGameGeekFetcher::ConfigWidget::saveConfigHook(KConfigGroup&) {
}

QString BoardGameGeekFetcher::ConfigWidget::preferredName() const {
  return BoardGameGeekFetcher::defaultName();
}

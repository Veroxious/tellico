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

#include "importdialog.h"
#include "document.h"
#include "tellico_kernel.h"
#include "tellico_debug.h"
#include "collection.h"

#include "translators/importer.h"
#include "translators/tellicoimporter.h"
#include "translators/bibteximporter.h"
#include "translators/bibtexmlimporter.h"
#include "translators/csvimporter.h"
#include "translators/xsltimporter.h"
#include "translators/audiofileimporter.h"
#include "translators/alexandriaimporter.h"
#include "translators/freedbimporter.h"
#include "translators/risimporter.h"
#include "translators/gcfilmsimporter.h"
#include "translators/filelistingimporter.h"
#include "translators/amcimporter.h"
#include "translators/griffithimporter.h"
#include "translators/pdfimporter.h"
#include "translators/referencerimporter.h"
#include "translators/deliciousimporter.h"

#include <klocale.h>
#include <kstandarddirs.h>

#include <qlayout.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qwhatsthis.h>
#include <qtimer.h>

// really according to taste or computer speed
const unsigned Tellico::Import::Importer::s_stepSize = 20;

using Tellico::ImportDialog;

ImportDialog::ImportDialog(Import::Format format_, const KURL::List& urls_, QWidget* parent_, const char* name_)
    : KDialogBase(parent_, name_, true /*modal*/, i18n("Import Options"), Ok|Cancel),
      m_coll(0),
      m_importer(importer(format_, urls_)) {
  QWidget* widget = new QWidget(this);
  QVBoxLayout* topLayout = new QVBoxLayout(widget, 0, spacingHint());

  QButtonGroup* bg = new QButtonGroup(1, Qt::Horizontal, i18n("Import Options"), widget);
  topLayout->addWidget(bg, 0);
  m_radioReplace = new QRadioButton(i18n("&Replace current collection"), bg);
  QWhatsThis::add(m_radioReplace, i18n("Replace the current collection with the contents "
                                       "of the imported file."));
  m_radioAppend = new QRadioButton(i18n("A&ppend to current collection"), bg);
  QWhatsThis::add(m_radioAppend, i18n("Append the contents of the imported file to the "
                                      "current collection. This is only possible when the "
                                      "collection types match."));
  m_radioMerge = new QRadioButton(i18n("&Merge with current collection"), bg);
  QWhatsThis::add(m_radioMerge, i18n("Merge the contents of the imported file to the "
                                     "current collection. This is only possible when the "
                                     "collection types match. Entries must match exactly "
                                     "in order to be merged."));
  if(m_importer->canImport(Kernel::self()->collectionType())) {
    // append by default?
    m_radioAppend->setChecked(true);
  } else {
    m_radioReplace->setChecked(true);
    m_radioAppend->setEnabled(false);
    m_radioMerge->setEnabled(false);
  }

  QWidget* w = m_importer->widget(widget, "importer_widget");
//  m_importer->readOptions(KGlobal::config());
  if(w) {
    topLayout->addWidget(w, 0);
  }

  connect(bg, SIGNAL(clicked(int)), m_importer, SLOT(slotActionChanged(int)));

  topLayout->addStretch();
  setMainWidget(widget);

  KGuiItem ok = KStdGuiItem::ok();
  ok.setText(i18n("&Import"));
  setButtonOK(ok);

  // want to grab default button action, too
  // since the importer might do something with widgets, don't just call it, do it after layout is done
  QTimer::singleShot(0, this, SLOT(slotUpdateAction()));
}

ImportDialog::~ImportDialog() {
  delete m_importer;
  m_importer = 0;
}

Tellico::Data::CollPtr ImportDialog::collection() {
  if(m_importer && !m_coll) {
    m_coll = m_importer->collection();
  }
  return m_coll;
}

QString ImportDialog::statusMessage() const {
  return m_importer ? m_importer->statusMessage() : QString::null;
}

Tellico::Import::Action ImportDialog::action() const {
  if(m_radioReplace->isChecked()) {
    return Import::Replace;
  } else if(m_radioAppend->isChecked()) {
    return Import::Append;
  } else {
    return Import::Merge;
  }
}

// static
Tellico::Import::Importer* ImportDialog::importer(Import::Format format_, const KURL::List& urls_) {
#define CHECK_SIZE if(urls_.size() > 1) kdWarning() << "ImportDialog::importer() - only importing first URL" << endl
  KURL firstURL = urls_.isEmpty() ? KURL() : urls_[0];
  Import::Importer* importer = 0;
  switch(format_) {
    case Import::TellicoXML:
      CHECK_SIZE;
      importer = new Import::TellicoImporter(firstURL);
      break;

    case Import::Bibtex:
      CHECK_SIZE;
      importer = new Import::BibtexImporter(urls_);
      break;

    case Import::Bibtexml:
      CHECK_SIZE;
      importer = new Import::BibtexmlImporter(firstURL);
      break;

    case Import::CSV:
      CHECK_SIZE;
      importer = new Import::CSVImporter(firstURL);
      break;

    case Import::XSLT:
      CHECK_SIZE;
      importer = new Import::XSLTImporter(firstURL);
      break;

    case Import::MODS:
      CHECK_SIZE;
      importer = new Import::XSLTImporter(firstURL);
      {
        QString xsltFile = locate("appdata", QString::fromLatin1("mods2tellico.xsl"));
        if(!xsltFile.isEmpty()) {
          KURL u;
          u.setPath(xsltFile);
          static_cast<Import::XSLTImporter*>(importer)->setXSLTURL(u);
        } else {
          kdWarning() << "ImportDialog::importer - unable to find mods2tellico.xml!" << endl;
        }
      }
      break;

    case Import::AudioFile:
      CHECK_SIZE;
      importer = new Import::AudioFileImporter(firstURL);
      break;

    case Import::Alexandria:
      CHECK_SIZE;
      importer = new Import::AlexandriaImporter();
      break;

    case Import::FreeDB:
      CHECK_SIZE;
      importer = new Import::FreeDBImporter();
      break;

    case Import::RIS:
      importer = new Import::RISImporter(urls_);
      break;

    case Import::GCfilms:
      CHECK_SIZE;
      importer = new Import::GCfilmsImporter(firstURL);
      break;

    case Import::FileListing:
      CHECK_SIZE;
      importer = new Import::FileListingImporter(firstURL);
      break;

    case Import::AMC:
      CHECK_SIZE;
      importer = new Import::AMCImporter(firstURL);
      break;

    case Import::Griffith:
      importer = new Import::GriffithImporter();
      break;

    case Import::PDF:
      importer = new Import::PDFImporter(urls_);
      break;

    case Import::Referencer:
      CHECK_SIZE;
      importer = new Import::ReferencerImporter(firstURL);
      break;

    case Import::Delicious:
      CHECK_SIZE;
      importer = new Import::DeliciousImporter(firstURL);
      break;

    case Import::GRS1:
      myDebug() << "ImportDialog::importer() - GRS1 not implemented" << endl;
      break;
  }
#ifndef NDEBUG
  if(!importer) {
    kdWarning() << "ImportDialog::importer() - importer not created!" << endl;
  }
#endif
  return importer;
#undef CHECK_SIZE
}

// static
QString ImportDialog::fileFilter(Import::Format format_) {
  QString text;
  switch(format_) {
    case Import::TellicoXML:
      text = i18n("*.tc *.bc|Tellico Files (*.tc)") + QChar('\n');
      text += i18n("*.xml|XML Files (*.xml)") + QChar('\n');
      break;

    case Import::Bibtex:
      text = i18n("*.bib|Bibtex Files (*.bib)") + QChar('\n');
      break;

    case Import::CSV:
      text = i18n("*.csv|CSV Files (*.csv)") + QChar('\n');
      break;

    case Import::Bibtexml:
    case Import::XSLT:
      text = i18n("*.xml|XML Files (*.xml)") + QChar('\n');
      break;

    case Import::MODS:
    case Import::Delicious:
      text = i18n("*.xml|XML Files (*.xml)") + QChar('\n');
      break;

    case Import::RIS:
      text = i18n("*.ris|RIS Files (*.ris)") + QChar('\n');
      break;

    case Import::GCfilms:
      text = i18n("*.gcs|GCstar Data Files (*.gcs)") + QChar('\n');
      text += i18n("*.gcf|GCfilms Data Files (*.gcf)") + QChar('\n');
      break;

    case Import::AMC:
      text = i18n("*.amc|AMC Data Files (*.amc)") + QChar('\n');
      break;

    case Import::PDF:
      text = i18n("*.pdf|PDF Files (*.pdf)") + QChar('\n');
      break;

    case Import::Referencer:
      text = i18n("*.reflib|Referencer Files (*.reflib)") + QChar('\n');
      break;

    case Import::AudioFile:
    case Import::Alexandria:
    case Import::FreeDB:
    case Import::FileListing:
    case Import::GRS1:
    case Import::Griffith:
      break;
  }

  return text + i18n("*|All Files");
}

// audio files are imported by directory
// alexandria is a defined location, as is freedb
// all others are files
Tellico::Import::Target ImportDialog::importTarget(Import::Format format_) {
  switch(format_) {
    case Import::AudioFile:
    case Import::FileListing:
      return Import::Dir;
    case Import::Griffith:
    case Import::Alexandria:
    case Import::FreeDB:
      return Import::None;
    default:
      return Import::File;
  }
}

Tellico::Import::FormatMap ImportDialog::formatMap() {
  // at one point, these were translated, but after some thought
  // I decided they were likely to be the same in any language
  // transliteration is unlikely
  Import::FormatMap map;
  map[Import::TellicoXML] = QString::fromLatin1("Tellico");
  map[Import::Bibtex]     = QString::fromLatin1("Bibtex");
  map[Import::Bibtexml]   = QString::fromLatin1("Bibtexml");
//  map[Import::CSV]        = QString::fromLatin1("CSV");
  map[Import::MODS]       = QString::fromLatin1("MODS");
  map[Import::RIS]        = QString::fromLatin1("RIS");
  map[Import::GCfilms]    = QString::fromLatin1("GCstar");
  map[Import::AMC]        = QString::fromLatin1("AMC");
  map[Import::Griffith]   = QString::fromLatin1("Griffith");
  map[Import::PDF]        = QString::fromLatin1("PDF");
  map[Import::Referencer] = QString::fromLatin1("Referencer");
  map[Import::Delicious ] = QString::fromLatin1("Delicious Library");
  return map;
}

QString ImportDialog::startDir(Import::Format format_) {
  if(format_ == Import::GCfilms) {
    QDir dir = QDir::home();
    // able to cd if exists and readable
    if(dir.cd(QString::fromLatin1(".local/share/gcfilms/"))) {
      return dir.absPath();
    }
  }
  return QString::fromLatin1(":import");
}

void ImportDialog::slotOk() {
  // some importers, like the CSV importer, can validate their settings
  if(!m_importer || m_importer->validImport()) {
    KDialogBase::slotOk();
  } else {
    myLog() << "ImportDialog::slotOk() - not a valid import" << endl;
  }
}

void ImportDialog::slotUpdateAction() {
  // distasteful hack
  // selectedId() is new in QT 3.2
//  m_importer->slotActionChanged(dynamic_cast<QButtonGroup*>(m_radioAppend->parentWidget())->selectedId());
  QButtonGroup* bg = static_cast<QButtonGroup*>(m_radioAppend->parentWidget());
  m_importer->slotActionChanged(bg->id(bg->selected()));
}

// static
Tellico::Data::CollPtr ImportDialog::importURL(Import::Format format_, const KURL& url_) {
  Import::Importer* imp = importer(format_, url_);
  Data::CollPtr c = imp->collection();
  if(!c && !imp->statusMessage().isEmpty()) {
    Kernel::self()->sorry(imp->statusMessage());
  }
  delete imp;
  return c;
}


#include "importdialog.moc"

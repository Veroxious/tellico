/****************************************************************************
**
** DCOP Skeleton generated by dcopidl2cpp from dcopinterface.kidl
**
** WARNING! All changes made in this file will be lost!
**
*****************************************************************************/

#include "./dcopinterface.h"

#include <kdatastream.h>
#include <qasciidict.h>

namespace Tellico {

static const int ApplicationInterface_fhash = 17;
static const char* const ApplicationInterface_ftable[16][3] = {
    { "bool", "importTellico(QString,QString)", "importTellico(QString file,QString action)" },
    { "bool", "importBibtex(QString,QString)", "importBibtex(QString file,QString action)" },
    { "bool", "importMODS(QString,QString)", "importMODS(QString file,QString action)" },
    { "bool", "importRIS(QString,QString)", "importRIS(QString file,QString action)" },
    { "bool", "exportXML(QString)", "exportXML(QString file)" },
    { "bool", "exportZip(QString)", "exportZip(QString file)" },
    { "bool", "exportBibtex(QString)", "exportBibtex(QString file)" },
    { "bool", "exportHTML(QString)", "exportHTML(QString file)" },
    { "bool", "exportCSV(QString)", "exportCSV(QString file)" },
    { "bool", "exportPilotDB(QString)", "exportPilotDB(QString file)" },
    { "QValueList<long int>", "selectedEntries()", "selectedEntries()" },
    { "QValueList<long int>", "filteredEntries()", "filteredEntries()" },
    { "void", "openFile(QString)", "openFile(QString file)" },
    { "void", "setFilter(QString)", "setFilter(QString text)" },
    { "bool", "showEntry(long int)", "showEntry(long int id)" },
    { 0, 0, 0 }
};
static const int ApplicationInterface_ftable_hiddens[15] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};

bool ApplicationInterface::process(const QCString &fun, const QByteArray &data, QCString& replyType, QByteArray &replyData)
{
    static QAsciiDict<int>* fdict = 0;
    if ( !fdict ) {
	fdict = new QAsciiDict<int>( ApplicationInterface_fhash, true, false );
	for ( int i = 0; ApplicationInterface_ftable[i][1]; i++ )
	    fdict->insert( ApplicationInterface_ftable[i][1],  new int( i ) );
    }
    int* fp = fdict->find( fun );
    switch ( fp?*fp:-1) {
    case 0: { // bool importTellico(QString,QString)
	QString arg0;
	QString arg1;
	QDataStream arg( data, IO_ReadOnly );
	if (arg.atEnd()) return false;
	arg >> arg0;
	if (arg.atEnd()) return false;
	arg >> arg1;
	replyType = ApplicationInterface_ftable[0][0]; 
	QDataStream _replyStream( replyData, IO_WriteOnly );
	_replyStream << importTellico(arg0, arg1 );
    } break;
    case 1: { // bool importBibtex(QString,QString)
	QString arg0;
	QString arg1;
	QDataStream arg( data, IO_ReadOnly );
	if (arg.atEnd()) return false;
	arg >> arg0;
	if (arg.atEnd()) return false;
	arg >> arg1;
	replyType = ApplicationInterface_ftable[1][0]; 
	QDataStream _replyStream( replyData, IO_WriteOnly );
	_replyStream << importBibtex(arg0, arg1 );
    } break;
    case 2: { // bool importMODS(QString,QString)
	QString arg0;
	QString arg1;
	QDataStream arg( data, IO_ReadOnly );
	if (arg.atEnd()) return false;
	arg >> arg0;
	if (arg.atEnd()) return false;
	arg >> arg1;
	replyType = ApplicationInterface_ftable[2][0]; 
	QDataStream _replyStream( replyData, IO_WriteOnly );
	_replyStream << importMODS(arg0, arg1 );
    } break;
    case 3: { // bool importRIS(QString,QString)
	QString arg0;
	QString arg1;
	QDataStream arg( data, IO_ReadOnly );
	if (arg.atEnd()) return false;
	arg >> arg0;
	if (arg.atEnd()) return false;
	arg >> arg1;
	replyType = ApplicationInterface_ftable[3][0]; 
	QDataStream _replyStream( replyData, IO_WriteOnly );
	_replyStream << importRIS(arg0, arg1 );
    } break;
    case 4: { // bool exportXML(QString)
	QString arg0;
	QDataStream arg( data, IO_ReadOnly );
	if (arg.atEnd()) return false;
	arg >> arg0;
	replyType = ApplicationInterface_ftable[4][0]; 
	QDataStream _replyStream( replyData, IO_WriteOnly );
	_replyStream << exportXML(arg0 );
    } break;
    case 5: { // bool exportZip(QString)
	QString arg0;
	QDataStream arg( data, IO_ReadOnly );
	if (arg.atEnd()) return false;
	arg >> arg0;
	replyType = ApplicationInterface_ftable[5][0]; 
	QDataStream _replyStream( replyData, IO_WriteOnly );
	_replyStream << exportZip(arg0 );
    } break;
    case 6: { // bool exportBibtex(QString)
	QString arg0;
	QDataStream arg( data, IO_ReadOnly );
	if (arg.atEnd()) return false;
	arg >> arg0;
	replyType = ApplicationInterface_ftable[6][0]; 
	QDataStream _replyStream( replyData, IO_WriteOnly );
	_replyStream << exportBibtex(arg0 );
    } break;
    case 7: { // bool exportHTML(QString)
	QString arg0;
	QDataStream arg( data, IO_ReadOnly );
	if (arg.atEnd()) return false;
	arg >> arg0;
	replyType = ApplicationInterface_ftable[7][0]; 
	QDataStream _replyStream( replyData, IO_WriteOnly );
	_replyStream << exportHTML(arg0 );
    } break;
    case 8: { // bool exportCSV(QString)
	QString arg0;
	QDataStream arg( data, IO_ReadOnly );
	if (arg.atEnd()) return false;
	arg >> arg0;
	replyType = ApplicationInterface_ftable[8][0]; 
	QDataStream _replyStream( replyData, IO_WriteOnly );
	_replyStream << exportCSV(arg0 );
    } break;
    case 9: { // bool exportPilotDB(QString)
	QString arg0;
	QDataStream arg( data, IO_ReadOnly );
	if (arg.atEnd()) return false;
	arg >> arg0;
	replyType = ApplicationInterface_ftable[9][0]; 
	QDataStream _replyStream( replyData, IO_WriteOnly );
	_replyStream << exportPilotDB(arg0 );
    } break;
    case 10: { // QValueList<long int> selectedEntries()
	replyType = ApplicationInterface_ftable[10][0]; 
	QDataStream _replyStream( replyData, IO_WriteOnly );
	_replyStream << selectedEntries( );
    } break;
    case 11: { // QValueList<long int> filteredEntries()
	replyType = ApplicationInterface_ftable[11][0]; 
	QDataStream _replyStream( replyData, IO_WriteOnly );
	_replyStream << filteredEntries( );
    } break;
    case 12: { // void openFile(QString)
	QString arg0;
	QDataStream arg( data, IO_ReadOnly );
	if (arg.atEnd()) return false;
	arg >> arg0;
	replyType = ApplicationInterface_ftable[12][0]; 
	openFile(arg0 );
    } break;
    case 13: { // void setFilter(QString)
	QString arg0;
	QDataStream arg( data, IO_ReadOnly );
	if (arg.atEnd()) return false;
	arg >> arg0;
	replyType = ApplicationInterface_ftable[13][0]; 
	setFilter(arg0 );
    } break;
    case 14: { // bool showEntry(long int)
	long int arg0;
	QDataStream arg( data, IO_ReadOnly );
	if (arg.atEnd()) return false;
	arg >> arg0;
	replyType = ApplicationInterface_ftable[14][0]; 
	QDataStream _replyStream( replyData, IO_WriteOnly );
	_replyStream << showEntry(arg0 );
    } break;
    default: 
	return DCOPObject::process( fun, data, replyType, replyData );
    }
    return true;
}

QCStringList ApplicationInterface::interfaces()
{
    QCStringList ifaces = DCOPObject::interfaces();
    ifaces += "Tellico::ApplicationInterface";
    return ifaces;
}

QCStringList ApplicationInterface::functions()
{
    QCStringList funcs = DCOPObject::functions();
    for ( int i = 0; ApplicationInterface_ftable[i][2]; i++ ) {
	if (ApplicationInterface_ftable_hiddens[i])
	    continue;
	QCString func = ApplicationInterface_ftable[i][0];
	func += ' ';
	func += ApplicationInterface_ftable[i][2];
	funcs << func;
    }
    return funcs;
}

} // namespace

#include <kdatastream.h>
#include <qasciidict.h>

namespace Tellico {

static const int CollectionInterface_fhash = 11;
static const char* const CollectionInterface_ftable[9][3] = {
    { "long int", "addEntry()", "addEntry()" },
    { "bool", "removeEntry(long int)", "removeEntry(long int entryID)" },
    { "QStringList", "values(QString)", "values(QString fieldName)" },
    { "QStringList", "values(long int,QString)", "values(long int entryID,QString fieldName)" },
    { "QStringList", "bibtexKeys()", "bibtexKeys()" },
    { "QString", "bibtexKey(long int)", "bibtexKey(long int entryID)" },
    { "bool", "setFieldValue(long int,QString,QString)", "setFieldValue(long int entryID,QString fieldName,QString value)" },
    { "bool", "addFieldValue(long int,QString,QString)", "addFieldValue(long int entryID,QString fieldName,QString value)" },
    { 0, 0, 0 }
};
static const int CollectionInterface_ftable_hiddens[8] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};

bool CollectionInterface::process(const QCString &fun, const QByteArray &data, QCString& replyType, QByteArray &replyData)
{
    static QAsciiDict<int>* fdict = 0;
    if ( !fdict ) {
	fdict = new QAsciiDict<int>( CollectionInterface_fhash, true, false );
	for ( int i = 0; CollectionInterface_ftable[i][1]; i++ )
	    fdict->insert( CollectionInterface_ftable[i][1],  new int( i ) );
    }
    int* fp = fdict->find( fun );
    switch ( fp?*fp:-1) {
    case 0: { // long int addEntry()
	replyType = CollectionInterface_ftable[0][0]; 
	QDataStream _replyStream( replyData, IO_WriteOnly );
	_replyStream << addEntry( );
    } break;
    case 1: { // bool removeEntry(long int)
	long int arg0;
	QDataStream arg( data, IO_ReadOnly );
	if (arg.atEnd()) return false;
	arg >> arg0;
	replyType = CollectionInterface_ftable[1][0]; 
	QDataStream _replyStream( replyData, IO_WriteOnly );
	_replyStream << removeEntry(arg0 );
    } break;
    case 2: { // QStringList values(QString)
	QString arg0;
	QDataStream arg( data, IO_ReadOnly );
	if (arg.atEnd()) return false;
	arg >> arg0;
	replyType = CollectionInterface_ftable[2][0]; 
	QDataStream _replyStream( replyData, IO_WriteOnly );
	_replyStream << values(arg0 );
    } break;
    case 3: { // QStringList values(long int,QString)
	long int arg0;
	QString arg1;
	QDataStream arg( data, IO_ReadOnly );
	if (arg.atEnd()) return false;
	arg >> arg0;
	if (arg.atEnd()) return false;
	arg >> arg1;
	replyType = CollectionInterface_ftable[3][0]; 
	QDataStream _replyStream( replyData, IO_WriteOnly );
	_replyStream << values(arg0, arg1 );
    } break;
    case 4: { // QStringList bibtexKeys()
	replyType = CollectionInterface_ftable[4][0]; 
	QDataStream _replyStream( replyData, IO_WriteOnly );
	_replyStream << bibtexKeys( );
    } break;
    case 5: { // QString bibtexKey(long int)
	long int arg0;
	QDataStream arg( data, IO_ReadOnly );
	if (arg.atEnd()) return false;
	arg >> arg0;
	replyType = CollectionInterface_ftable[5][0]; 
	QDataStream _replyStream( replyData, IO_WriteOnly );
	_replyStream << bibtexKey(arg0 );
    } break;
    case 6: { // bool setFieldValue(long int,QString,QString)
	long int arg0;
	QString arg1;
	QString arg2;
	QDataStream arg( data, IO_ReadOnly );
	if (arg.atEnd()) return false;
	arg >> arg0;
	if (arg.atEnd()) return false;
	arg >> arg1;
	if (arg.atEnd()) return false;
	arg >> arg2;
	replyType = CollectionInterface_ftable[6][0]; 
	QDataStream _replyStream( replyData, IO_WriteOnly );
	_replyStream << setFieldValue(arg0, arg1, arg2 );
    } break;
    case 7: { // bool addFieldValue(long int,QString,QString)
	long int arg0;
	QString arg1;
	QString arg2;
	QDataStream arg( data, IO_ReadOnly );
	if (arg.atEnd()) return false;
	arg >> arg0;
	if (arg.atEnd()) return false;
	arg >> arg1;
	if (arg.atEnd()) return false;
	arg >> arg2;
	replyType = CollectionInterface_ftable[7][0]; 
	QDataStream _replyStream( replyData, IO_WriteOnly );
	_replyStream << addFieldValue(arg0, arg1, arg2 );
    } break;
    default: 
	return DCOPObject::process( fun, data, replyType, replyData );
    }
    return true;
}

QCStringList CollectionInterface::interfaces()
{
    QCStringList ifaces = DCOPObject::interfaces();
    ifaces += "Tellico::CollectionInterface";
    return ifaces;
}

QCStringList CollectionInterface::functions()
{
    QCStringList funcs = DCOPObject::functions();
    for ( int i = 0; CollectionInterface_ftable[i][2]; i++ ) {
	if (CollectionInterface_ftable_hiddens[i])
	    continue;
	QCString func = CollectionInterface_ftable[i][0];
	func += ' ';
	func += CollectionInterface_ftable[i][2];
	funcs << func;
    }
    return funcs;
}

} // namespace


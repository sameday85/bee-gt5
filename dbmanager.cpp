#include "qdatetime.h"
#include "dbmanager.h"
#include "qvariant.h"


DbManager::DbManager(QString dbName)
{
    mDbName = dbName;
}

DbManager::~DbManager() {
    if (mDb.isOpen())
        mDb.close();
}

void DbManager::createDbIf() {
    const QString DRIVER("QSQLITE");
    if(!QSqlDatabase::isDriverAvailable(DRIVER))
        return;
    mDb = QSqlDatabase::addDatabase(DRIVER);
    mDb.setDatabaseName(mDbName);
    if (!mDb.open())
        return;
    if (!mDb.tables().contains(QLatin1String("users"))) {
        QSqlQuery query(mDb);
        query.exec("CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT)");
        query.exec("CREATE TABLE dicts (id INTEGER PRIMARY KEY, name TEXT)");
        query.exec("CREATE TABLE stats (id INTEGER PRIMARY KEY, time INTEGER, user_id INTEGER, dict_id INTEGER, asked INTEGER, answered INTEGER)");
    }
}

qlonglong DbManager::insertUserIf(QString userName) {
    if (!mDb.isOpen())
        return 0;
    qlonglong id = 0;
    QSqlQuery query(mDb);
    query.prepare("SELECT * FROM users where name=?");
    query.addBindValue(userName);
    query.exec();
    if (query.next()) {
        id = query.value(0).toLongLong();
    }
    else {
        query.prepare("INSERT INTO users (name) VALUES (?)");
        query.addBindValue(userName);
        query.exec();
        id = query.lastInsertId().toLongLong();
    }
    query.finish();

    return id;
}

qlonglong DbManager::insertDictIf(QString dictName) {
    if (!mDb.isOpen())
        return 0;
    qlonglong id = 0;
    QSqlQuery query(mDb);
    query.prepare("SELECT * FROM dicts where name=?");
    query.addBindValue(dictName);
    query.exec();
    if (query.next()) {
        id = query.value(0).toLongLong();
    }
    else {
        query.prepare("INSERT INTO dicts (name) VALUES (?)");
        query.addBindValue(dictName);
        query.exec();
        id = query.lastInsertId().toLongLong();
    }
    query.finish();

    return id;
}

qlonglong DbManager::insertStat(qlonglong userId, qlonglong dictId, int asked, int answered) {
    if (asked <= 0 || userId <= 0 || dictId <= 0)
        return 0;
    QSqlQuery query(mDb);
    query.prepare("INSERT INTO stats (time, user_id, dict_id, asked, answered) VALUES (?,?,?,?,?)");
    query.addBindValue(QDateTime::currentDateTime().toMSecsSinceEpoch());
    query.addBindValue(userId);
    query.addBindValue(dictId);
    query.addBindValue(asked);
    query.addBindValue(answered);
    query.exec();

    return query.lastInsertId().toLongLong();
}

void DbManager::updateStat(qlonglong id, int asked, int answered) {
    QSqlQuery query(mDb);
    query.prepare("UPDATE stats SET asked=?, answered=? where id=?");
    query.addBindValue(asked);
    query.addBindValue(answered);
    query.addBindValue(id);
    query.exec();
}

#include "qdatetime.h"
#include "dbmanager.h"
#include "qvariant.h"

#define MS_IN_DAY       (1000*60*60*24)

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
        query.exec("CREATE TABLE progs (id INTEGER PRIMARY KEY, user_id INTEGER, dict_id INTEGER, grade INTEGER, pos INTEGER)");
        query.exec("CREATE TABLE stats (id INTEGER PRIMARY KEY, time INTEGER, day INTEGER, user_id INTEGER, dict_id INTEGER, asked INTEGER, answered INTEGER, active INTEGER default 1)");
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
    qlonglong ms = QDateTime::currentDateTime().toMSecsSinceEpoch();
    qlonglong day = ms / MS_IN_DAY;
    QSqlQuery query(mDb);
    query.prepare("INSERT INTO stats (time, day, user_id, dict_id, asked, answered) VALUES (?,?,?,?,?,?)");
    query.addBindValue(ms);
    query.addBindValue(day);
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

 Statistics* DbManager::findDictStatsBy(qlonglong userId, qlonglong dictId) {
     Statistics* stats = new Statistics();

     QSqlQuery query(mDb);
     query.prepare("select sum(asked), sum(answered) from stats where user_id=? and dict_id=? and active=1");
     query.addBindValue(userId);
     query.addBindValue(dictId);
     query.exec();
     if (query.next()) {
         stats->setAsked(query.value(0).toInt());
         stats->setAnswered(query.value(1).toInt());
     }
     query.finish();

     return stats;
 }

 Statistics* DbManager::findLifetimeStatsBy(qlonglong userId) {
     Statistics* stats = new Statistics();

     QSqlQuery query(mDb);
     query.prepare("select sum(asked), sum(answered) from stats where user_id=?");
     query.addBindValue(userId);
     query.exec();
     if (query.next()) {
         stats->setAsked(query.value(0).toInt());
         stats->setAnswered(query.value(1).toInt());
     }
     query.finish();

     return stats;
 }

 QVector<Statistics*>* DbManager::findStatsBy(qlonglong userId) {
    QVector<Statistics*> *all = new QVector<Statistics*>;

    QSqlQuery query(mDb);
    query.prepare("select min(time), sum(asked), sum(answered) from stats where user_id=? group by day order by day desc limit 7");
    query.addBindValue(userId);
    query.exec();
    while (query.next()) {
        qlonglong time = query.value(0).toLongLong();
        int asked = query.value(1).toInt();
        int answered = query.value(2).toInt();
        all->insert(0, new Statistics(time, asked, answered));
    }
    query.finish();

    return all;
}

int DbManager::getPositionBy(qlonglong userId, qlonglong dictId, int grade, int def) {
    int position = def;

    QSqlQuery query(mDb);
    query.prepare("select pos from progs where user_id=? and dict_id=? and grade=?");
    query.addBindValue(userId);
    query.addBindValue(dictId);
    query.addBindValue(grade);
    query.exec();
    if (query.next()) {
        position = query.value(0).toInt();
    }
    query.finish();

    return position;
}

void DbManager::updatePositionBy(qlonglong userId, qlonglong dictId, int grade, int pos) {
    int old = getPositionBy(userId, dictId, grade, -1);
    if (old >= 0) {
        QSqlQuery query(mDb);
        query.prepare("update progs set pos=? where user_id=? and dict_id=? and grade=?");
        query.addBindValue(pos);
        query.addBindValue(userId);
        query.addBindValue(dictId);
        query.addBindValue(grade);
        query.exec();
    }
    else {
        QSqlQuery query(mDb);
        query.prepare("insert into progs (user_id, dict_id, grade, pos) values (?,?,?,?)");
        query.addBindValue(userId);
        query.addBindValue(dictId);
        query.addBindValue(grade);
        query.addBindValue(pos);
        query.exec();
    }
}

void DbManager::resetDictStatsBy(qlonglong userId, qlonglong dictId) {
    QSqlQuery query(mDb);
    query.prepare("update stats set active=0 where user_id=? and dict_id=?");
    query.addBindValue(userId);
    query.addBindValue(dictId);
    query.exec();
}


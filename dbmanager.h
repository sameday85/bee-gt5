#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>

class DbManager
{
public:
    DbManager(QString dbName);
    ~DbManager();
    void createDbIf();

    qlonglong insertUserIf(QString userName);
    qlonglong insertDictIf(QString dictName);
    qlonglong insertStat(qlonglong userId, qlonglong dictId, int asked, int answered);
    void updateStat(qlonglong id, int asked, int answered);
private:
    QString mDbName;
    QSqlDatabase mDb;
};

#endif // DBMANAGER_H

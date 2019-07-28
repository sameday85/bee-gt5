#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <qvector.h>
#include "statistics.h"

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
    QVector<Statistics*>* findDailyStatsBy(qlonglong userId);
    Statistics* findDictStatsBy(qlonglong userId, qlonglong dictId);
    Statistics* findLifetimeStatsBy(qlonglong userId);
    void resetDictStatsBy(qlonglong userId, qlonglong dictId);
    int getPositionBy(qlonglong userId, qlonglong dictId, int grade, int mode,  int def);
    void updatePositionBy(qlonglong userId, qlonglong dictId, int grade, int mode, int pos);

private:
    QString mDbName;
    QSqlDatabase mDb;
};

#endif // DBMANAGER_H

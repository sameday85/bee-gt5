#ifndef STATISTICS_H
#define STATISTICS_H

#include <QString>
#include <QHash>
#include "common.h"

class Statistics
{
public:
    Statistics();
    void loadHistoryStats(QString user);
    void loadDictStats(QString user, QString dictionary);
    void save();

private:
    QHash<QString, int> stats;
    QString statsFilePath;

    int asked, correct;
private:
    void load();

public:
    int getWordIndexLastPracticed(QString dict, int grade);
    void setWordIndexLastPracticed(QString dict, int grade, int index);

    void reset();
    int getAsked();
    void incAsked();
    int getCorrect();
    void incCorrect();
    int getCorrectPercentage();
};

#endif // STATISTICS_H

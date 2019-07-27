#ifndef STATISTICS_H
#define STATISTICS_H

#include <QString>
#include <QHash>
#include "common.h"

class Statistics
{
public:
    Statistics();
    Statistics(qlonglong time, int asked, int answered);

private:
    QHash<QString, int> stats;
    QString statsFilePath;

    int asked, answered;
    qlonglong time;

public:
    int getWordIndexLastPracticed(QString dict, int grade);
    void setWordIndexLastPracticed(QString dict, int grade, int index);

    void reset();
    int getAsked();
    void setAsked(int value);
    void incAsked();
    int getAnswered();
    void setAnswered(int value);
    void incAnswered();
    qlonglong getTime();
    void setTime (qlonglong tm);
    float getCorrectPercentage();
};

#endif // STATISTICS_H

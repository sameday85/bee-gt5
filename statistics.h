#ifndef STATISTICS_H
#define STATISTICS_H

#include <QString>
#include <QHash>
#include "common.h"

class Statistics
{
public:
    Statistics();
    void load(QString user);
    void save();

private:
    QHash<QString, int> stats;

    QString statsFilePath;
    QString mUsername;

    int asked, correct;
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

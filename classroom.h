#ifndef CLASSROOM_H
#define CLASSROOM_H

#include <QString>
#include <QVector>
#include <QTextStream>

#include "common.h"
#include "wordex.h"
#include "speaker.h"
#include "statistics.h"

class ClassRoom : public QObject
{
    Q_OBJECT
public:
    explicit ClassRoom(QString &username, QString& dictionary, int& mode);
    int prepare(int forGrade, int progress);
    int adjustGrade(int grade);
    void dismiss();
    int getTotalWordsSelected();
    int getProgress();
    WordEx* getCurrentWord();

    Statistics *getStatistic();
    int getFinishedGrade();

private:
    void loadDictionary();
    void say(QString mp3);
    void chooseWords(int max, bool random);
    void downloadWordOnlineIf(WordEx *word);

private:
    Speaker speaker;
    Statistics stats;
    QVector<WordEx*> mWordList;
    WordEx *currentWord;
    QString mUsername;
    QString mDictionary;
    int mGrade;
    int mMode;

    bool multipleGradeSupported;
    bool started;
    int index[1024];
    int selectedTotal;
    int selected;

    int failures;
    bool tried;

public slots:
    int present();
    int onAnswer(QString anwer);
};

#endif // CLASSROOM_H

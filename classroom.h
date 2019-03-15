#ifndef CLASSROOM_H
#define CLASSROOM_H

#include <QString>
#include <QVector>
#include <QTextStream>

#include "common.h"
#include "word.h"
#include "speaker.h"

class ClassRoom : public QObject
{
    Q_OBJECT
public:
    explicit ClassRoom(QString &username, QString& dictionary, int& mode);
    int prepare(int forGrade);
    void dismiss();
    Word* getCurrentWord();

private:
    void loadDictionary();
    void loadV10 (QTextStream &in);
    void loadV11 (QTextStream &in);
    void say(QString mp3);

private:
    Speaker speaker;
    QVector<Word> mWordList;
    Word currentWord;
    QString mUsername;
    QString mDictionary;
    int mMode;

    int index[1024];
    int selectedTotal;
    int selected;

    int failures;

public slots:
    int present();
    int onAnswer(QString anwer);
};

#endif // CLASSROOM_H

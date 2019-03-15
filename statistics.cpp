#include "QFile"
#include "QTextStream"
#include "statistics.h"

#define KEY_ASKED       "asked"
#define KEY_CORRECT     "correct"


Statistics::Statistics()
{
    reset();
}

void Statistics::reset() {
    asked = correct = 0;
}

int Statistics::getAsked() {
    return asked;
}

void Statistics::incAsked() {
    ++asked;
}

int Statistics::getCorrect() {
    return correct;
}

void Statistics::incCorrect() {
    ++correct;
}

int Statistics::getCorrectPercentage() {
    if (asked <= 0)
        return 0;
    return 100 * correct / asked;
}

void Statistics::load(QString user) {
    mUsername = user;
    stats.clear();
    statsFilePath=QString::asprintf("%s/%s.txt", szAppCacheDir, mUsername.toStdString().c_str());

    QFile file(statsFilePath);
    if (!file.open(QIODevice::ReadOnly))
        return;

   QTextStream in(&file);
   while (!in.atEnd()) {
       QString line = in.readLine();
       int pos = line.indexOf("=");
       if (pos > 0) {
           QString key = line.left(pos);
           int value = line.mid(pos+1).toInt();
           stats[key]=value;

           if (key == KEY_ASKED)
               asked = value;
           else if (key == KEY_CORRECT)
               correct = value;
       }
   }
   file.close();
}

void Statistics::save() {
    stats[KEY_ASKED]=asked;
    stats[KEY_CORRECT]=correct;

    QFile file(statsFilePath);
    if (!file.open(QIODevice::WriteOnly))
        return;

    QTextStream out(&file);
    QHash<QString, int>::const_iterator i = stats.constBegin();
    while (i != stats.constEnd()) {
        out << i.key() << "=" << i.value() << endl;
        ++i;
    }
    file.close();
}

int Statistics::getWordIndexLastPracticed(QString dict, int grade) {
    QString key=dict + "@" + QString::number(grade);
    if (stats.contains(key))
        return stats[key];
    return 0;
}

void Statistics::setWordIndexLastPracticed(QString dict, int grade, int index) {
    QString key=dict + "@" + QString::number(grade);
    stats[key]=index;
}

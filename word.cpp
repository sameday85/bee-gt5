#include "word.h"

Word::Word() {
    mSpelling="";
    mGrade = 0;
    triedDownload = false;
}

Word::Word(QString _word)
{
    mSpelling = _word;
    mGrade = 0;
    triedDownload = false;
}

bool Word::getTriedDownload() const
{
    return triedDownload;
}

void Word::setTriedDownload(bool value)
{
    triedDownload = value;
}

void Word::setGrade(int _grade) {
    mGrade = _grade;
}

int Word::getGrade() const {
    return mGrade;
}

QString Word::getSpelling() {
    return mSpelling;
}

#ifndef WORD_H
#define WORD_H

#include <QString>

class Word
{
public:
    Word();
    Word(QString _word);

protected:
    QString mSpelling;

private:
    int mGrade;
    bool triedDownload;

public:
    int getGrade() const;
    void setGrade (int grade);

    bool getTriedDownload() const;
    void setTriedDownload(bool value);

    QString getSpelling();
};

#endif // WORD_H

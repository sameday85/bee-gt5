#ifndef WORDEX_H
#define WORDEX_H

#include <QString>
#include <QList>
#include "word.h"
#include "speaker.h"
#include "common.h"

class WordSense {
public:
    WordSense(QString definition, QString example);
    QString getDefinition() const;
    QString getExample() const;
private:
    QString mDefinition;
    QString mExample;

};

class WordCategory {
public:
    WordCategory(QString category, QString audio);
    ~WordCategory();
    void addSense(WordSense * sense);

    QString getCategory() const;
    QString getAudio() const;
    QList<WordSense*> *getSenses();

private:
    QString mCategory;
    QString mAudio;
    QList<WordSense*> senses;
};

class WordEx : public Word
{
public:
    WordEx(QString _word);
    ~WordEx();

    void addCategory(WordCategory *category);
    void resetCategory();
    QList<WordCategory *> *getCategories();

    //shortcuts to access the first elements
    QString getCategory();
    QString getDefinition();
    QString getSample();
    QString getAudio();

    void pronounceWord();
    void pronounceWordAlt();
    void pronounceCategory(bool online);
    void pronounceDefinition(bool online);
    void pronounceSample(bool online);

private:
    Speaker speaker;
    QList<WordCategory *>categories;
};

#endif // WORDEX_H

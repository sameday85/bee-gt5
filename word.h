#ifndef WORD_H
#define WORD_H

#include <QString>
#include "speaker.h"
#include "common.h"

class Word
{
public:
    Word();
    Word(QString _word);

    void setCategory(QString _category);
    void setDefinition(QString _definition);
    void setSample(QString _sample);
    void setAudio(QString _audio);
    void setGrade(int _grade);

private:
    Speaker speaker;
    int grade;
    QString word;
    QString category;
    QString definition;
    QString sample;
    QString audio;

    QString parse_json(QString json, QString tag);
    bool download_soundoftext(QString word_or_sentence, QString filename);

public:
    int getGrade() const;
    QString getSpelling();
    QString getCategory();
    QString getDefinition();
    QString getSample();

    void pronounceWord();
    void pronounceWordAlt();
    void pronounceCategory(bool online);
    void pronounceDefinition(bool online);
    void pronounceSample(bool online);
};

#endif // WORD_H

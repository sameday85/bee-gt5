#include "wordex.h"

WordSense::WordSense(QString definition, QString example) {
    mDefinition = definition;
    mExample = example;
}

QString WordSense::getDefinition() const
{
    return mDefinition;
}

QString WordSense::getExample() const
{
    return mExample;
}

//=========================================================
WordCategory::WordCategory(QString category, QString audio) {
    mCategory = category;
    mAudio = audio;
}

WordCategory::~WordCategory() {
    for (int i = 0; i < senses.count(); ++i)
        delete senses.at(i);
    senses.clear();
}

void WordCategory::addSense(WordSense *sense) {
    senses.append(sense);
}

QList<WordSense*> *WordCategory::getSenses() {
    return &senses;
}

QString WordCategory::getCategory() const
{
    return mCategory;
}

QString WordCategory::getAudio() const
{
    return mAudio;
}

//======================================================
WordEx::WordEx(QString _word) {
    mSpelling = _word;
}

WordEx::~WordEx() {
    for (int i = 0; i < categories.count(); ++i)
        delete categories.at(i);
    categories.clear();
}

QString WordEx::getSpelling() {
    return mSpelling;
}

void WordEx::addCategory(WordCategory *category) {
    categories.append(category);
}

QList<WordCategory *> *WordEx::getCategories() {
    return &categories;
}

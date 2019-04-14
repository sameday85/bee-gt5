#include "wordex.h"

WordSense::WordSense() {
}

QString WordSense::getDefinition() const
{
    return mDefinition;
}

void WordSense::setDefinition(const QString &definition)
{
    mDefinition = definition;
}

QString WordSense::getExample() const
{
    return mExample;
}

void WordSense::setExample(const QString &example)
{
    mExample = example;
}
//=========================================================
WordCategory::WordCategory(QString category) {
    mCategory = category;
}

WordCategory::~WordCategory() {
    for (int i = 0; i < senses.count(); ++i)
        delete senses.at(i);
    senses.clear();
}

void WordCategory::addSense(WordSense *sense) {
    senses.add(sense);
}

QArray<WordSense*> *WordCategory::getSenses() {
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

void WordCategory::setAudio(const QString &audio)
{
    mAudio = audio;
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

void WordEx::addCategory(WordCategory *category) {
    categories.add(category);
}

QList<WordCategory *> *WordEx::getCategories() {
    return &categories;
}

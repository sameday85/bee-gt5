#include "wordex.h"
#include "urlconnection.h"

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
WordEx::WordEx(QString _word) : Word(_word) {
}

WordEx::~WordEx() {
    for (int i = 0; i < categories.count(); ++i)
        delete categories.at(i);
    categories.clear();
}

void WordEx::addCategory(WordCategory *category) {
    categories.append(category);
}

QList<WordCategory *> *WordEx::getCategories() {
    return &categories;
}

QString WordEx::getCategory() {
    return categories.isEmpty() ? "" : categories.at(0)->getCategory();
}

QString WordEx::getDefinition() {
    if (categories.isEmpty())
        return "";
    if (categories.at(0)->getSenses()->isEmpty())
        return "";
    return categories.at(0)->getSenses()->at(0)->getDefinition();
}

QString WordEx::getSample(){
    if (categories.isEmpty())
        return "";
    if (categories.at(0)->getSenses()->isEmpty())
        return "";
    return categories.at(0)->getSenses()->at(0)->getExample();
}

QString WordEx::getAudio() {
    return categories.isEmpty() ? "" : categories.at(0)->getAudio();
}

//http://ssl.gstatic.com/dictionary/static/sounds/oxford/persistent--_us_1.mp3
void WordEx::pronounceWord() {
    QString audio = getAudio();
    if (mSpelling.isEmpty())
        return;
    QString url = audio.isEmpty() ? QString::asprintf("http://ssl.gstatic.com/dictionary/static/sounds/oxford/%s--_us_1.mp3", mSpelling.toStdString().c_str()) : audio;
    //cached voice file name (mp3)
    QString voice = QString::asprintf("%s/%s.mp3", szAppCacheDir, mSpelling.toStdString().c_str());

    bool mp3_available = false;
    QFile file(voice);
    if (!file.exists()) {
        UrlConnection urlConnection;
        if (urlConnection.download2File(url, voice)) {
            mp3_available = true;
        }
        else if (speaker.download_soundoftext(mSpelling, voice)) {
            mp3_available = true;
        }
    }
    else {
        mp3_available = true;
    }

    if (mp3_available) {
        speaker.play_offline(voice);
    }
    else {
        speaker.play_online(url);
    }
}

void WordEx::pronounceWordAlt() {
    speaker.read_sentence_offline(mSpelling);
}

void WordEx::pronounceCategory(bool online) {
    QString category = getCategory();
    if (online)
        speaker.read_sentence_online(category);
    else
        speaker.read_sentence_offline(category);
}

void WordEx::pronounceDefinition(bool online) {
    QString definition = getDefinition();
    if (online)
        speaker.read_sentence_online(definition);
    else
        speaker.read_sentence_offline(definition);
}

void WordEx::pronounceSample(bool online) {
    QString sample = getSample();
    if (online)
        speaker.read_sentence_online(sample);
    else
        speaker.read_sentence_offline(sample);
}


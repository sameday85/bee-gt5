#include <QFile>
#include "word.h"
#include "urlconnection.h"
//======================================================================
//======================================================================
//======================================================================

Word::Word() {
    word="";
}

Word::Word(QString _word)
{
    word = _word;
}

void Word::setSpelling(QString _word) {
    word = _word;
}

void Word::setCategory(QString _category) {
    category = _category;
}

void Word::setDefinition(QString _definition) {
    definition=_definition;
}

void Word::setSample(QString _sample) {
    sample = _sample;
}

void Word::setAudio(QString _audio) {
    audio = _audio;
}

void Word::setGrade(int _grade) {
    grade = _grade;
}

QString Word::getSpelling() {
    return word;
}

QString Word::getCategory() {
    return category;
}

QString Word::getSample() {
    return sample;
}

QString Word::getDefinition() {
    return definition;
}

int Word::getGrade() const {
    return grade;
}

//http://ssl.gstatic.com/dictionary/static/sounds/oxford/persistent--_us_1.mp3
void Word::pronounceWord() {
    if (word.isEmpty())
        return;
    QString url = audio.isEmpty() ? QString::asprintf("http://ssl.gstatic.com/dictionary/static/sounds/oxford/%s--_us_1.mp3", word.toStdString().c_str()) : audio;
    //cached voice file name (mp3)
    QString voice = QString::asprintf("%s/%s.mp3", szAppCacheDir, word.toStdString().c_str());

    bool mp3_available = false;
    QFile file(voice);
    if (!file.exists()) {
        UrlConnection urlConnection;
        if (urlConnection.download2File(url, voice)) {
            mp3_available = true;
        }
        else if (speaker.download_soundoftext(word, voice)) {
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

void Word::pronounceWordAlt() {
    speaker.read_sentence_offline(word);
}

void Word::pronounceCategory(bool online) {
    if (online)
        speaker.read_sentence_online(category);
    else
        speaker.read_sentence_offline(category);
}

void Word::pronounceDefinition(bool online) {
    if (online)
        speaker.read_sentence_online(definition);
    else
        speaker.read_sentence_offline(definition);
}

void Word::pronounceSample(bool online) {
    if (online)
        speaker.read_sentence_online(sample);
    else
        speaker.read_sentence_offline(sample);
}




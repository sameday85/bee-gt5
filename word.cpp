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
        else if (download_soundoftext(word, voice)) {
            mp3_available = true;
        }
    }
    else {
        mp3_available = true;
    }

    if (mp3_available) {
        speaker.play(voice);
    }
    else {
        speaker.play_online(url);
    }
}

void Word::pronounceCategory() {
    speaker.read_sentence_online(category);
}

void Word::pronounceDefinition() {
    speaker.read_sentence_online(definition);
}

void Word::pronounceSample() {
    speaker.read_sentence_online(sample);
}


QString Word::parse_json(QString json, QString tag) {
    QString value;

    int pos = json.indexOf(tag);
    if (pos >= 0) {
        pos += tag.length();
        int pos1 = json.indexOf("\"", pos) + 1;
        int pos2 = json.indexOf("\"", pos1);
        value = json.mid(pos1, pos2 - pos1);
    }

    return value;
}

bool Word::download_soundoftext(QString word_or_sentence, QString filename) {
    QString data = QString::asprintf("{\r\n\"engine\": \"Google\",\r\n\"data\": {\r\n\"text\": \"%s\",\r\n\"voice\": \"en-US\"}\r\n}\r\n", word_or_sentence.toStdString().c_str());

    QString url = "https://api.soundoftext.com/sounds";
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    UrlConnection urlConnection;
    QNetworkReply *reply = urlConnection.doPost(request, data.toUtf8());
    QString response = urlConnection.isError() ? "" : urlConnection.saveToString(reply);

    QString value = parse_json(response, "\"id\"");
    if (value.isEmpty())
        return false;

    QString url2 = QString::asprintf("https://api.soundoftext.com/sounds/%s", value.toStdString().c_str());
    response = urlConnection.download2String(url2);
    int wait = 4; //4 seconds
    value = parse_json(response, "\"location\"");
    while (value.isEmpty()) {
        if (--wait < 0)
            break;
        QThread::sleep(1);//seconds
        response = urlConnection.download2String(url2);
    }
    if (value.isEmpty())
        return false;
    return urlConnection.download2File(value, filename);
}


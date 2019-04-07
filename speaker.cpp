#include <QThread>
#include "urlconnection.h"
#include "speaker.h"
#include "common.h"

Speaker::Speaker()
{

}

int Speaker::play_offline( QString audio_file) {
    char command[256];
    int status;
    /* create command to execute */
    if (audio_file.endsWith(".wav"))
        sprintf( command, "omxplayer \"%s\" > /dev/null", audio_file.toStdString().c_str());
    else {
        sprintf( command, "mpg321 -q \"%s\"", audio_file.toStdString().c_str());
    }
    /* play sound */
    status = system( command );
    return status;
}

//Aassume the url points to a mp3 audio
void Speaker::play_online(QString url) {
    char command[255];
    sprintf(command, "mpg321 -q \"%s\"", url.toStdString().c_str());
    system(command );//QProcess::execute
}

/* speech.sh
---------------------------------------------------------------------------------------------------------------------------------
#!/bin/bash
say() { local IFS=+;/usr/bin/mpg321 -q "http://translate.google.com/translate_tts?ie=UTF-8&client=tw-ob&q=$*&tl=en"; }
say $*
---------------------------------------------------------------------------------------------------------------------------------
./speech.sh The jacket is red
pico2wave -w sentence.wav "The jacket is red" && omxplayer sentence.wav >/dev/null
*/
//when play audio online, omxplayer always stopps early
void Speaker::read_sentence_online(QString sentence) {
    if (sentence.isEmpty())
        return;
    sentence = sanityCheck(sentence);
    char command[512];
    sprintf(command, "%s/speech.sh \"%s\"", szApplicationDir, sentence.toStdString().c_str());
    system(command );
}

void Speaker::read_sentence_offline(QString sentence) {
    if (sentence.isEmpty())
        return;

    sentence = sanityCheck(sentence);
    char command[512];
    sprintf(command, "pico2wave -w /tmp/sentence.wav \"%s\" && omxplayer /tmp/sentence.wav>/dev/null", sentence.toStdString().c_str());
    system(command );
}

QString Speaker::sanityCheck(QString sentence) {
    sentence = sentence.replace("'", "");
    sentence = sentence.replace("\"", "");
    sentence = sentence.replace("\\", "");

    return sentence;
}

QString Speaker::parse_json(QString json, QString tag) {
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

bool Speaker::download_soundoftext(QString word_or_sentence, QString filename) {
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

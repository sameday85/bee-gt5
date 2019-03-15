#include <QThread>
#include "speaker.h"
#include "common.h"

Speaker::Speaker()
{

}

int Speaker::play( QString audio_file) {
    char command[256];
    int status;
    /* create command to execute */
    sprintf( command, "omxplayer -o local %s >/dev/null", audio_file.toStdString().c_str());

    /* play sound */
    status = system( command );
    return status;
}

void Speaker::play_online(QString url) {
    char command[255];
    sprintf(command, "omxplayer %s >/dev/null", url.toStdString().c_str());
    system(command );//QProcess::execute
}

/* speech.sh
---------------------------------------------------------------------------------------------------------------------------------
#!/bin/bash
say() { local IFS=+;/usr/bin/omxplayer >/dev/null "http://translate.google.com/translate_tts?ie=UTF-8&client=tw-ob&q=$*&tl=en"; }
say $*
---------------------------------------------------------------------------------------------------------------------------------
./speech.sh The jacket is red
pico2wave -w sentence.wav "The jacket is red" && omxplayer sentence.wav >/dev/null
*/
void Speaker::read_sentence_online(QString sentence) {
    if (sentence.isEmpty())
        return;

    char command[512];
    sprintf(command, "%s/speech.sh \"%s\"", szApplicationDir, sentence.toStdString().c_str());
    system(command );
}

void Speaker::read_sentence_offline(QString sentence) {
    if (sentence.isEmpty())
        return;

    char command[512];
    sprintf(command, "pico2wave -w sentence.wav \"%s\" && omxplayer sentence.wav >/dev/null", sentence.toStdString().c_str());
    system(command );
}


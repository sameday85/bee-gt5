#ifndef SPEAKER_H
#define SPEAKER_H

#include <QString>

class Speaker
{
public:
    Speaker();
    int play(QString audio_file);
    void play_online(QString url);
    void read_sentence_online(QString sentence);
    void read_sentence_offline(QString sentence);

private:
    QString sanityCheck(QString);
};

#endif // SPEAKER_H

#ifndef SPEAKER_H
#define SPEAKER_H

#include <QString>

class Speaker
{
public:
    Speaker();
    int play_offline(QString audio_file);
    void play_online(QString url);
    void read_sentence_online(QString sentence);
    void read_sentence_offline(QString sentence);
    bool download_soundoftext(QString word_or_sentence, QString filename);

private:
    QString sanityCheck(QString);
    QString parse_json(QString json, QString tag);

};

#endif // SPEAKER_H

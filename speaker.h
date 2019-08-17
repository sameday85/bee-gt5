#ifndef SPEAKER_H
#define SPEAKER_H

#include <QtCore>
#include <QString>
#include <QMediaPlayer>

class Speaker : public QObject
{
    Q_OBJECT

public:
    Speaker();
    virtual ~Speaker();

    int play_offline(QString audio_file);
    void play_online(QString url);
    void read_sentence_online(QString sentence);
    void read_sentence_offline(QString sentence);
    bool download_soundoftext(QString word_or_sentence, QString filename);

private:
    QString sanityCheck(QString);
    QString parse_json(QString json, QString tag);

    QMediaPlayer *player;
    QEventLoop *loop;

private slots:
    void stateChanged(QMediaPlayer::State state);
};

#endif // SPEAKER_H

#ifndef CALLPLAYER_H
#define CALLPLAYER_H

#include <QObject>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QFile>
#include <QBuffer>
#include <QThread>

class CallPlayer : public QObject
{
    Q_OBJECT

    QMediaPlayer *player;
    QMediaPlaylist *playlist;
    QBuffer *ringtoneBuf;

public slots:
    void run();
    void start(bool _loop);
    void stop();

    int getPlaylistSize(); // playlist will be changed to select a ringtone in the settings

signals:
    void signalPlay();
    void signalStop();
    void signalSetPlaylist(QMediaPlaylist*);


public:
    CallPlayer();
    ~CallPlayer();

    void setTrackIndex(int idx);
};

#endif // CALLPLAYER_H

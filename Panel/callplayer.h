#ifndef CALLPLAYER_H
#define CALLPLAYER_H

#include <QObject>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QThread>

class CallPlayer : public QObject
{
    Q_OBJECT

    QMediaPlayer *player;
    QMediaPlaylist *playlist;

public slots:
    void run();
    void start(bool _loop);
    void stop();

    void setTrackIndex(int idx);
    int getPlaylistSize();

signals:
    void setPlayList(QMediaPlaylist*);
    void signalPlay();
    void signalStop();


public:
    CallPlayer();
    ~CallPlayer();
};

#endif // CALLPLAYER_H

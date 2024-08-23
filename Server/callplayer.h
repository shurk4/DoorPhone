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

    int getPlaylistSize(); // playlist will be changed to select a ringtone in the settings

signals:
    void signalPlay();
    void signalStop();


public:
    CallPlayer();
    ~CallPlayer();

    void setTrackIndex(int idx);
};

#endif // CALLPLAYER_H

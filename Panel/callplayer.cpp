#include "callplayer.h"

CallPlayer::CallPlayer()
{

    qDebug() << "--- Call player starting!";
    player = new QMediaPlayer(this);
    playlist = new QMediaPlaylist(this);

    playlist->addMedia(QUrl("qrc:/ringtones/1.mp3"));
    playlist->addMedia(QUrl("qrc:/ringtones/2.mp3"));
    playlist->addMedia(QUrl("qrc:/ringtones/3.mp3"));
    playlist->addMedia(QUrl("qrc:/ringtones/4.mp3"));
    playlist->addMedia(QUrl("qrc:/ringtones/5.mp3"));
    qDebug() << "--- OK!";
}

void CallPlayer::setTrackIndex(int idx)
{
    playlist->setCurrentIndex(idx);
}

void CallPlayer::run()
{
    qDebug() << "--- Call player started in thread: " << QThread::currentThreadId();

    playlist->setCurrentIndex(3);

    player->setPlaylist(playlist);
}

void CallPlayer::start(bool _loop)
{    
    if(_loop)
    {
        qDebug() << "Start in loop";
        playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
    }
    else
    {
        qDebug() << "Start once";
        playlist->setPlaybackMode(QMediaPlaylist::CurrentItemOnce);
    }
    qDebug() << "--- Start call sound";
    player->play();
}

void CallPlayer::stop()
{
    qDebug() << "--- Stop call sound";
    player->stop();
}

int CallPlayer::getPlaylistSize()
{
    return playlist->mediaCount();
}

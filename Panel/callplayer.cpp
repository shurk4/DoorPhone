#include "callplayer.h"

CallPlayer::CallPlayer()
{

    qDebug() << "--- Call player starting!";
    player = new QMediaPlayer();
    playlist = new QMediaPlaylist();

    playlist->addMedia(QUrl("qrc:/ringtones/1.mp3"));
    playlist->addMedia(QUrl("qrc:/ringtones/2.mp3"));
    playlist->addMedia(QUrl("qrc:/ringtones/3.mp3"));
    playlist->addMedia(QUrl("qrc:/ringtones/4.mp3"));
    playlist->addMedia(QUrl("qrc:/ringtones/5.mp3"));
    qDebug() << "--- OK!";
}

CallPlayer::~CallPlayer()
{
    player->deleteLater();
    playlist->deleteLater();
}

void CallPlayer::setTrackIndex(int idx)
{
    playlist->setCurrentIndex(idx);
}

void CallPlayer::run()
{
    qDebug() << "--- Call player started in thread: " << QThread::currentThreadId();

    playlist->setCurrentIndex(3);

//    player->setPlaylist(playlist);
    connect(this, &CallPlayer::setPlayList, player, &QMediaPlayer::setPlaylist);
    connect(this, &CallPlayer::signalPlay, player, &QMediaPlayer::play);
    connect(this, &CallPlayer::signalStop, player, &QMediaPlayer::stop);
    emit setPlayList(playlist);
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
    emit signalPlay();
}

void CallPlayer::stop()
{
    qDebug() << "--- Stop call sound";
    emit signalStop();
}

int CallPlayer::getPlaylistSize()
{
    return playlist->mediaCount();
}

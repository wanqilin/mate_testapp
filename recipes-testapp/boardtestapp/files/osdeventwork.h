#ifndef OSDEVENTWORK_H
#define OSDEVENTWORK_H

#include <QObject>
#include <QMediaPlayer>
#include <QMediaRecorder>
#include <QAudioRecorder>
#if 0 //#ifdef OS_UNIX
#include <QMediaEncoderSettings>
#endif
#include <QAudioEncoderSettings>
#include <QFileDialog>

#ifdef OS_UNIX
#include <gst/gst.h>
#endif

class OsdEventWork : public QObject
{
    Q_OBJECT

public:
    OsdEventWork(QWidget *parent = nullptr);
    ~OsdEventWork();

private:
    QString m_sAudiofileName;
#ifdef OS_WINDOWS
    QMediaPlayer *audioplayer;
    QMediaRecorder *audiorecorder;
    QAudioRecorder  *m_pAudioRecorder;
#endif
#ifdef OS_UNIX
    GstElement *Recordpipeline;
    GstElement *playingPipeline;
#endif

public slots:
    void recordAudio(QString *sAudiofileName);
    void playAudio(QString *sAudiofileName);
    void stopRecording(void);

signals:
    void RefreshdurationChanged(const qint64 duration);
    void RefreshPlayStatus(const QMediaPlayer::State newState);
    void RefreshMediaPlayStatus(const QMediaPlayer::MediaStatus newState);

private slots:
    void onDurationChanged(qint64 duration);
    void onStateChanged();
    void onMediaStateChanged();
};
#endif // OSDEVENTWORK_H

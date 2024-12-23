#ifndef OSDEVENTWORK_H
#define OSDEVENTWORK_H

#include <QObject>
#include <QTimer>
#include <QMediaPlayer>
#include <QMediaRecorder>
#include <QAudioRecorder>
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
    QTimer *AudioRecordtimer;    
#ifdef OS_WINDOWS
    QMediaPlayer *audioplayer;
    QMediaRecorder *audiorecorder;
    QAudioRecorder  *m_pAudioRecorder;
#endif
#ifdef OS_UNIX
    GstElement *playingPipeline;
    GstElement *Recordpipeline;
#endif

public slots:
    void recordAudio(QString *sAudiofileName);
    void playAudio(QString *sAudiofileName);
    void stopRecording(void);

signals:
    void RefreshdurationChanged(const qint64 duration);
    void RefreshPlayStatus(const qint8 newState);  // 0:stop 1:playing 2:pause
    void RefreshMediaPlayStatus(const qint8 newState);

private slots:
    void onDurationChanged(qint64 duration);
    void onStateChanged();
    void onMediaStateChanged();
#if OS_UNIX
    void RecordedDuration();
#endif
};
#endif // OSDEVENTWORK_H

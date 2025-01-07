#include "osdeventwork.h"
#include <QPushButton>
#include <QtDebug>

OsdEventWork::OsdEventWork(QWidget *parent)
{
    qDebug() << "OsdEventThread is run!";
    #ifdef OS_UNIX
    // Init Gstreamer lib
    gst_init(nullptr, nullptr);
    Recordpipeline = nullptr;
    playingPipeline = nullptr;

    AudioRecordtimer = new QTimer(this);
    AudioRecordtimer->setSingleShot(false);  //Non-single trigger
    AudioRecordtimer->setInterval( 1*1000 );   
    connect(AudioRecordtimer,&QTimer::timeout, this,&OsdEventWork::RecordedDuration);
    #else
    //init class
    audioplayer = new QMediaPlayer(static_cast<QWidget*>(parent));
    audiorecorder = new QMediaRecorder(audioplayer,static_cast<QWidget*>(parent));
    m_pAudioRecorder = new QAudioRecorder (static_cast<QWidget*>(parent));
    
    for ( QString &device : m_pAudioRecorder->audioInputs())
    {
        qDebug() << "Audiodevice:" << device;
    }
    

    for ( QString &codecName : m_pAudioRecorder->supportedAudioCodecs () )
    {
        qDebug() << "AudiocodecName:" << codecName;
    }
    connect(audioplayer, &QMediaPlayer::stateChanged, this, &OsdEventWork::onStateChanged);
    connect(audioplayer, &QMediaPlayer::mediaStatusChanged, this, &OsdEventWork::onMediaStateChanged);
    connect(m_pAudioRecorder, &QAudioRecorder::durationChanged, this, &OsdEventWork::onDurationChanged);
    #endif
}

OsdEventWork::~OsdEventWork()
{
    qDebug() << "~OsdEventThread!";
    stopRecording();
  
    #if OS_UNIX
    if (AudioRecordtimer)
    {
        AudioRecordtimer->stop();
        delete AudioRecordtimer; 
    }     
    if (Recordpipeline) {
        gst_object_unref(Recordpipeline);
    }
    if (playingPipeline) {
        gst_object_unref(playingPipeline);
    }
    #endif
}

void OsdEventWork::onStateChanged(void)
{
    #ifdef OS_WINDOWS
    qDebug() << "audioplayer onStateChange" << audioplayer->state();
    emit RefreshPlayStatus(audioplayer->state());
    #endif
}

void OsdEventWork::onMediaStateChanged(void)
{
    #ifdef OS_WINDOWS
    qDebug() << "audioplayer onMediaStateChanged" << audioplayer->mediaStatus();
    emit RefreshMediaPlayStatus((qint8)audioplayer->mediaStatus());
    #endif
}

void OsdEventWork::onDurationChanged(qint64 duration)
{
    #ifdef OS_WINDOWS
    emit RefreshdurationChanged(duration);
    #endif
}

 #if OS_UNIX
void OsdEventWork::RecordedDuration()
 {
        GstFormat fmt = GST_FORMAT_TIME;
        gint64 position = 0;
        gboolean ret = gst_element_query_position(Recordpipeline, fmt, &position);

        if (ret) {
            qDebug() << "Recorded duration:" << position / GST_MSECOND << "mseconds";
            emit RefreshdurationChanged(position / GST_MSECOND);
        } else {
            qDebug() << "Failed to get position.";
        }
    }
#endif

void OsdEventWork:: recordAudio(QString *sAudiofileName)
{
    m_sAudiofileName = *sAudiofileName;
    #if OS_UNIX
    stopRecording();

    QString pipelineDescription =
        "autoaudiosrc ! volume volume=5.0 ! audioconvert ! audioresample ! "
        "wavenc ! filesink location=" + m_sAudiofileName;

    Recordpipeline = gst_parse_launch(pipelineDescription.toUtf8().constData(), nullptr);
    if (!Recordpipeline) {
         qDebug() << "Failed to create recording pipeline";
        return;
    }
    AudioRecordtimer->start();    
    gst_element_set_state(Recordpipeline, GST_STATE_PLAYING);
    emit RefreshdurationChanged(0);
    qDebug() << "Recording Audio started..." << m_sAudiofileName;

    #else
    if (!m_sAudiofileName.isEmpty()) {
        if (!m_sAudiofileName.endsWith(".wav", Qt::CaseInsensitive))
        {
            m_sAudiofileName += ".wav";
        }

        m_pAudioRecorder->setOutputLocation(QUrl::fromLocalFile(m_sAudiofileName));
        m_pAudioRecorder->setAudioInput(m_pAudioRecorder->defaultAudioInput());

        QAudioEncoderSettings AudioSettings;
        AudioSettings.setCodec("audio/pcm");
        AudioSettings.setSampleRate(44100);
        AudioSettings.setChannelCount(2);
        AudioSettings.setBitRate(128000);
        AudioSettings.setQuality(QMultimedia::HighQuality);
        m_pAudioRecorder->setAudioSettings(AudioSettings);

        QAudioEncoderSettings Settings;
        Settings= m_pAudioRecorder->audioSettings();

        if (m_pAudioRecorder->state() == QMediaRecorder::StoppedState){
            m_pAudioRecorder->record();
            qDebug() << "Recording started to" << m_sAudiofileName;
            qDebug() << "Current recorder state:" << m_pAudioRecorder->state();
        }
        else{
            qDebug() << "Recorder is already in use or an error occurred.";
        }
    }
    else {
        qDebug() << "Recording cancelled.";
    }
    #endif
}

void OsdEventWork::playAudio(QString *sAudiofileName)
{
    m_sAudiofileName = *sAudiofileName;
#if OS_UNIX
    QString pipelineDescription =
            "filesrc location=" + m_sAudiofileName + " ! wavparse ! audioconvert ! volume volume=6.0 ! autoaudiosink";

    playingPipeline = gst_parse_launch(pipelineDescription.toUtf8().constData(), nullptr);
    if (!playingPipeline) {
        qDebug() << "Failed to create playback pipeline";
        return;
    }

    gst_element_set_state(playingPipeline, GST_STATE_PLAYING);
    qDebug() << "Playing audio...";
    emit RefreshPlayStatus(1); //playing

    GstBus *bus = gst_element_get_bus(playingPipeline);
    GstMessage *msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
                                                    (GstMessageType)(GST_MESSAGE_EOS | GST_MESSAGE_ERROR));
    if (msg) {
        gst_message_unref(msg);
    }
    gst_object_unref(bus);

    gst_element_set_state(playingPipeline, GST_STATE_NULL);
    emit RefreshMediaPlayStatus(7);

    qDebug() << "Playback finished.";
#else
    if (!m_sAudiofileName.isEmpty()) {
        audioplayer->setMedia(QUrl::fromLocalFile(m_sAudiofileName));
        audioplayer->play();
        qDebug() << "Playing audio from" << m_sAudiofileName;
    }
#endif    
}

void OsdEventWork::stopRecording(void)
{
    #if OS_UNIX
    if (AudioRecordtimer)
    {
        AudioRecordtimer->stop();
        //delete AudioRecordtimer;
    }    
    if (Recordpipeline)
    {
        GstStateChangeReturn ret = gst_element_set_state(Recordpipeline, GST_STATE_NULL);
        if (ret == GST_STATE_CHANGE_FAILURE) {
            qDebug() << "Failed to stop the recording pipeline.";
        }
        qDebug() << "Stop Recording..." << m_sAudiofileName;
    }
    if (playingPipeline)
    {
        GstStateChangeReturn ret = gst_element_set_state(playingPipeline, GST_STATE_NULL);
        if (ret == GST_STATE_CHANGE_FAILURE) {
            qDebug() << "Failed to stop the recording pipeline.";
        }
        qDebug() << "Stop Playing..." << m_sAudiofileName;
    }

    #else
    if (m_pAudioRecorder->state() == QMediaRecorder::RecordingState) {
        m_pAudioRecorder->stop();
        if (QFile::exists(m_sAudiofileName)) {
            qDebug() << "Recording successfully saved to:" << m_sAudiofileName;
        } else {
            qDebug() << "Recording failed. File not found at:" << m_sAudiofileName;
        }
    } else {
        qDebug() << "No active recording to stop.";
    }
    #endif
}


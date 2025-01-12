#include <QApplication>
#include "mainwindow.h"
#include "OpenCVWindow.h"
#include <QTimer>
#include <QtDebug>
#include <QDateTime>
#include <QPalette>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QSettings>

using namespace std;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    //, pOpenCVWindow(nullptr)
{
    this->setWindowTitle(tr("NxpTestApp"));
    this->setGeometry(0,0,APP_WIDTH,APP_HEIGH);

    InitVariable();

    DrawOSDInterface();

    pOsdEventThread = new QThread();
    pOsdEventWork = new OsdEventWork(this);
    pOsdEventWork->moveToThread(pOsdEventThread);
    pOsdEventThread->start();

    pWirelessDeviceWorkThread = new WirelessDeviceWorkThread();
    pWirelessDeviceWorkThread->start();

    pHotPlugWorkThread = new HotPlugWorkThread();
    pHotPlugWorkThread->start();
#if(CAMERA_MODE_TYPE == 1)
#else
    pOpenCVCameraThread = new OpenCVCameraThread();
    pOpenCVCameraThread->start();
#endif
    SetSignalAndSLot();
    AppLicCheck();
}

MainWindow::~MainWindow()
{
#if (APP_LIC_TYPE==0)
    AppLicCnt = loadData();
    if(AppLicCnt > APP_DEMO_LIC_CNT)
    {
        qDebug()<<"AppLic Cnt over Limit!";
        AppLicCnt = APP_DEMO_LIC_CNT + 2;
        saveData(AppLicCnt);
    }
#endif
    m_timer->stop();
    AppExittimer->stop();
    delete m_timer;
    delete AppExittimer;

    pWirelessDeviceWorkThread->stop();
    pWirelessDeviceWorkThread->quit();
    pWirelessDeviceWorkThread->wait();
    pWirelessDeviceWorkThread->deleteLater();

    pHotPlugWorkThread->stop();
    pHotPlugWorkThread->quit();
    pHotPlugWorkThread->wait();
    pHotPlugWorkThread->deleteLater();
#if(CAMERA_MODE_TYPE == 1)
#else
    pOpenCVCameraThread->stop();
    pOpenCVCameraThread->quit();
    pOpenCVCameraThread->wait();
    pOpenCVCameraThread->deleteLater();
#endif
    pOsdEventWork->deleteLater();
    pOsdEventThread->quit();
    pOsdEventThread->wait();
    pOsdEventThread->deleteLater();
    qDebug() << "~MainWindow()";
}

#if(APP_LIC_TYPE==0)
void MainWindow::saveData(int value) {
    QSettings settings("BoardTestApp", "BoardTestAppConfig");
    settings.setValue("AppLicCnt", value);
    qDebug() << "Data saved:" << value;
}

int MainWindow::loadData() {
    QSettings settings("BoardTestApp", "BoardTestAppConfig");
    int value = settings.value("AppLicCnt", 0).toInt();
    qDebug() << "Data loaded:" << value;
    return value;
}

void MainWindow::AppExit()
{
    qDebug() << "AppExit!";
    QApplication::quit();
    qDebug() << "AppQuit-2!";
}

void MainWindow::AppLicLimitshowInfo() {
    qDebug() << "AppLic Limit ShowInfo!";
    QMessageBox::warning(this, "DemoApp", "Trial times has exceeded the upper limit,Exit the App after 10 seconds", QMessageBox::Ok);
    //SetTimer
    AppExittimer = new QTimer(this);
    AppExittimer->setSingleShot(true);  //Non-single trigger
    AppExittimer->setInterval( 10*1000 );
    connect(AppExittimer, SIGNAL(timeout()), this, SLOT(AppExit()));
    AppExittimer->start();
}
#endif

void MainWindow::AppLicCheck(void)
{
#if(APP_LIC_TYPE==0)
    if(AppLicCnt > APP_DEMO_LIC_CNT)
        emit DemoAppLicLimit();
#endif
}
void MainWindow::InitVariable(void)
{
#if(APP_LIC_TYPE==0)
    AppLicCnt = loadData();
    if(AppLicCnt <= APP_DEMO_LIC_CNT)
    {
        AppLicCnt = AppLicCnt + 1;
        saveData(AppLicCnt);
    }
#endif
}

void MainWindow::DrawOSDInterface(void)
{
    appbox = new QGroupBox(this);
    appbox->setGeometry(0,0,APP_WIDTH,APP_HEIGH);
    applayout = new QBoxLayout(QBoxLayout::TopToBottom,this);
#if(APP_LIC_TYPE==1)
    this->displayTitle = new QLabel("TestApp",this);
    this->displayTitle->setGeometry(530,50,200,50);
#else
    this->displayTitle = new QLabel("TestApp.demo",this);
    this->displayTitle->setGeometry(530,50,200,50);
#endif

    this->displayTitle->setFont(QFont("Arial",16,QFont::Bold));
    //applayout->addWidget(displayTitle);
    //appbox->setLayout(applayout);

    //Draw Clock
    DrawClockPage();

    //draw wifi
    DrawWifiPage();

    //draw BT
    DrawBtPage();

    //Draw Event Listen
    DrawEventListenPage();

    //Draw Audio
    DrawAudioPage();

    //draw camera
    DrawCameraPage();
}

void MainWindow::SetSignalAndSLot(void)
{
    //AppLicType
#if(APP_LIC_TYPE==0)
    connect(this,&MainWindow::DemoAppLicLimit,this,&MainWindow::AppLicLimitshowInfo);
#endif

#if(CAMERA_MODE_TYPE == 1)
#else
    //opencv slot
    connect(pOpenCVCameraThread,&OpenCVCameraThread::frameProcessed,this,&MainWindow::displayImage);
#endif
    //wifi slot
    connect(pWirelessDeviceWorkThread, &WirelessDeviceWorkThread::RefreshWifiOSD, this, &MainWindow::wifiListUpdate);
    //Bt slot
    connect(pWirelessDeviceWorkThread, &WirelessDeviceWorkThread::RefreshBtOSD, this, &MainWindow::btListUpdate);
    //Hot Plug Slot
    connect(pHotPlugWorkThread, &HotPlugWorkThread::RefreshUsbOSD, this, &MainWindow::UsbDeviceUpdate);
    connect(pHotPlugWorkThread, &HotPlugWorkThread::RefreshEthOSD, this, &MainWindow::DrawlanStatusUpdate);
    //Audio slot
    connect(AudioRecordButton, &QPushButton::clicked, this, &MainWindow::AudioRecordClicked);
    connect(stopRecordButton, &QPushButton::clicked, this, &MainWindow::AudioStopClicked);
    connect(AudioPlayButton, &QPushButton::clicked, this, &MainWindow::AudioPlayClicked);
    connect(this,&MainWindow::AudioRecordClickedSignal,pOsdEventWork,&OsdEventWork::recordAudio);
    connect(this,&MainWindow::AudioStopClickedSignal,pOsdEventWork,&OsdEventWork::stopRecording);
    connect(this,&MainWindow::AudioPlayClickedSignal,pOsdEventWork,&OsdEventWork::playAudio);
    connect(pOsdEventWork,&OsdEventWork::RefreshdurationChanged,this,&MainWindow::AudioRecordDurationUpdate);
    connect(pOsdEventWork,&OsdEventWork::RefreshPlayStatus,this,&MainWindow::AudioPlayStatusUpdate);
    connect(pOsdEventWork,&OsdEventWork::RefreshMediaPlayStatus,this,&MainWindow::AudioPlayMediaStatusUpdate);
}

void MainWindow::PrintText(const QString &text)
{
    qDebug()<<text;
}

void MainWindow::DrawClockPage(void)
{
    //draw lcdnumber
    this->lcdnumber = new QLCDNumber(19,this);
    this->lcdnumber->setSegmentStyle(QLCDNumber::Flat);
    this->lcdnumber->setGeometry(500,160,180,50);
    //applayout->addWidget(lcdnumber);
    //appbox->setLayout(applayout);

    //SetTimer
    m_timer = new QTimer(this);
    m_timer->setSingleShot(false);  //Non-single trigger
    m_timer->setInterval( 1*1000 );
    connect(m_timer, SIGNAL(timeout()), this, SLOT(ClockUpdate()));
    m_timer->start();
}

void MainWindow::ClockUpdate(void)
{
    //Get Current Time
    QDateTime dt = QDateTime::currentDateTime();
    QString strTime = dt.toString("yyyy-MM-dd HH:mm:ss");
    this->lcdnumber->display(strTime);
}
void MainWindow::DrawCameraPage(void)
{
    CameraGroupBox = new QGroupBox("Camera",this);
    CameraGroupBox->setGeometry(700,20,580,460);
    Cameralayout = new QVBoxLayout(this);

    QPalette palette;
    palette.setColor(QPalette::Window,QColor(50, 50, 50));
    palette.setColor(QPalette::WindowText, Qt::white);
    //Camera View
    #if(CAMERA_MODE_TYPE == 1)
    CameraViewVideoFrame = new QVideoWidget(this);
    CameraViewVideoFrame->setFixedSize(560,420);
    CameraViewVideoFrame->setAutoFillBackground(true);
    #else
    CameraView = new QLabel("No Data!",this);
    CameraView->setPalette(palette);
    CameraView->setFixedSize(540,405);
    CameraView->setAutoFillBackground(true);
    CameraView->setPalette(palette);
    CameraView->setAlignment(Qt::AlignCenter);
    #endif
    //MatchImage
    /*
    MatchImage = new QLabel("No Data!",this);
    MatchImage->setPalette(palette);
    MatchImage->setFixedSize(320,240);
    MatchImage->setAutoFillBackground(true);
    MatchImage->setPalette(palette);
    MatchImage->setAlignment(Qt::AlignCenter);
    */
    //CaptureButton
    /*
    captureButton= new QPushButton("Capture", this);
    captureButton->resize(180,50);
    */
   //OpenCVButton
    //OpenCVButton = new QPushButton("GotoOpenCV",this);
    //OpenCVButton->resize(180,50);
    //connect(OpenCVButton,&QPushButton::clicked,this,&MainWindow::GotoOpenCVWindow);
#if(CAMERA_MODE_TYPE == 1)
    Cameralayout->addWidget(CameraViewVideoFrame);
#else
    Cameralayout->addWidget(CameraView);
#endif
    //Cameralayout->addWidget(MatchImage);
    //Cameralayout->addWidget(captureButton);
    //Cameralayout->addWidget(OpenCVButton);

    CameraGroupBox->setLayout(Cameralayout);

    //applayout->addWidget(CameraGroupBox);
    //appbox->setLayout(applayout);

    //startOpenCVfaceRecognition
#if(CAMERA_MODE_TYPE == 1)    
    CameraInit();
#endif    
}

#if(CAMERA_MODE_TYPE == 1)  
void MainWindow::CameraInit(void)
{
    const QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    if (!cameras.isEmpty())
    {
        qDebug()<<"Camera deviceName:";
        qDebug()<<cameras.first().deviceName();
        //Creat CameraObject
        camera = new QCamera(cameras.first(),this);

        QPalette palette;
        palette.setColor(QPalette::Window,QColor(50, 50, 50));
        //Creat Viewfinder
        //viewfinder = new QCameraViewfinder(this);
        //viewfinder->setFixedSize(320,240);
        //viewfinder->setAutoFillBackground(true);
        //viewfinder->setPalette(palette);
        //Cameralayout->addWidget(viewfinder);
        //CameraGroupBox->setLayout(Cameralayout);
        
        QCameraViewfinderSettings viewfinderSettings;
        viewfinderSettings.setResolution(640,480);
        viewfinderSettings.setMaximumFrameRate(15.0);
        viewfinderSettings.setMaximumFrameRate(30.0);
        camera->setViewfinderSettings(viewfinderSettings);

        //camer connect Viewfinder
        camera->setViewfinder(CameraViewVideoFrame);

        //Creat ImageCapture Object
        //imageCapture = new QCameraImageCapture(camera, this);
        //connect(captureButton,&QPushButton::clicked,this,&MainWindow::captureImage);
        //connect(imageCapture, &QCameraImageCapture::imageCaptured, this, &MainWindow::displayImage);
        camera->start();
    }
    else
    {
        qDebug()<<"No camera available.";
        CameraView->setText("No camera available.");
    }
}
#endif

void MainWindow::captureImage() {
    // CaptureImage
    imageCapture->capture();
}

void MainWindow::displayImage(int id, const QImage &preview)
{
    Q_UNUSED(id);
    CameraView->clear();
    CameraView->setScaledContents(true);
    CameraView->setPixmap(QPixmap::fromImage(preview));
}

/*
void MainWindow::GotoOpenCVWindow()
{
    if(!pOpenCVWindow)
    {
        qDebug()<<"CreateOpenCVWindow!";
        pOpenCVWindow = new OpenCVWindow(this);
        connect(pOpenCVWindow, &QWidget::destroyed, this, [this]() { pOpenCVWindow = nullptr; });
    }
    qDebug()<<"OpenCVWindowShow!";
    pOpenCVWindow->show();
    pOpenCVWindow->raise();    //front desk
    pOpenCVWindow->activateWindow();
}
*/

void MainWindow::DrawWifiPage(void)
{
    QGroupBox *wifiGroupBox = new QGroupBox("Wifi List",this);
    wifiGroupBox->setGeometry(10,20,220,280);
    QVBoxLayout *Wifilayout = new QVBoxLayout(this);
    WifilistWidget = new QListWidget(this);
    WifilistWidget->setFixedSize(150, 200);
    WifilistWidget->setMinimumSize(150, 200);
    WifilistWidget->setMaximumSize(200, 220);
    WifilistWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    WifilistWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    Wifilayout->addWidget(WifilistWidget);
    wifiGroupBox->setLayout(Wifilayout);
    //applayout->addWidget(wifiGroupBox);
    //appbox->setLayout(applayout);

}

void MainWindow::wifiListUpdate(const QStringList &wifiList)
{
    //qDebug()<<"wifi list update!";
    this->WifilistWidget->clear();

    // get wifi list
    for (const QString &wifi : wifiList) {
        //qDebug()<<wifi;
        this->WifilistWidget->addItem(wifi);
    }
}

void MainWindow::btListUpdate(const QStringList &btList)
{
    qDebug()<<"Bt list update!";
    this->BtlistWidget->clear();

    // get bt list
    for (const QString &bt : btList) {
        //qDebug()<<bt;
        this->BtlistWidget->addItem(bt);
    }
}

void MainWindow::DrawBtPage(void)
{
    QGroupBox *BtGroupBox = new QGroupBox("BT List",this);
    BtGroupBox->setGeometry(240,20,220,280);
    QVBoxLayout *Btlayout = new QVBoxLayout(this);
    BtlistWidget = new QListWidget(this);
    BtlistWidget->setFixedSize(150, 200);
    BtlistWidget->setMinimumSize(150, 200);
    BtlistWidget->setMaximumSize(200, 220);
    BtlistWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    BtlistWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    Btlayout->addWidget(BtlistWidget);
    BtGroupBox->setLayout(Btlayout);
    //applayout->addWidget(wifiGroupBox);
    //appbox->setLayout(applayout);

}

void MainWindow::DrawEventListenPage(void)
{
    QGroupBox *ListenEventBox = new QGroupBox("Event",this);
    ListenEventBox->setGeometry(10,320,120,240);
    QVBoxLayout *Eventlayout = new QVBoxLayout(this);

    //Lan
    lanbox = new QGroupBox(this);
    QLabel *lantxt = new QLabel("LAN",this);
    lanstatus = new QLabel(this);
    lanstatus->setFixedSize(20, 20);
    lanstatus->setStyleSheet("background-color: gray; border-radius: 10px;");
    QHBoxLayout *lanlayout = new QHBoxLayout(this);
    lanlayout->addWidget(lantxt);
    lanlayout->addWidget(lanstatus);
    //lanlayout->addStretch();
    lanbox->setLayout(lanlayout);

    //USB
    usbbox = new QGroupBox(this);
    usbtxt = new QLabel("USB",this);
    usbstatus = new QLabel("0",this);
    usbstatus->setFixedSize(20, 20);
    usbstatus->setAlignment(Qt::AlignCenter);
    usbstatus->setStyleSheet("color: black; background-color: gray; border-radius: 10px;");
    QHBoxLayout *usblayout = new QHBoxLayout(this);
    usblayout->addWidget(usbtxt);
    usblayout->addWidget(usbstatus);
    //usb1layout->addStretch();
    usbbox->setLayout(usblayout);

    Eventlayout->addWidget(lanbox);
    Eventlayout->addWidget(usbbox);
    Eventlayout->addStretch();
    ListenEventBox->setLayout(Eventlayout);

    //applayout->addWidget(ListenEventBox);
    //appbox->setLayout(applayout);

}

void MainWindow::DrawlanStatusUpdate(bool isOnline)
{
    if (isOnline) {
        //qDebug() << "Network connected!";
        lanstatus->setStyleSheet("background-color: green; border-radius: 10px;");
    } else {
        //qDebug() << "Network disconnected!";
        lanstatus->setStyleSheet("background-color: gray; border-radius: 10px;");
    }
}

void MainWindow::UsbDeviceUpdate(int usbCnt)
{
    //qDebug()<<"Usb Cnt:"<<usbCnt;
    if(usbCnt>0)
        usbstatus->setStyleSheet("color: white; background-color: green; border-radius: 10px;");
    else
        usbstatus->setStyleSheet("color: black; background-color: gray; border-radius: 10px;");
    usbstatus->setNum(usbCnt);
}

void MainWindow::AudioRecordClicked(void)
{
#ifdef OS_WINDOWS
    QString outputFile = "audio_output.wav";
    AudioRecordfileName = QFileDialog::getSaveFileName(this, "Save Audio File", outputFile, "*.wav");
#else
    AudioRecordfileName = "/usr/bin/audio_output.wav";
#endif
    emit AudioRecordClickedSignal(&AudioRecordfileName);
}

void MainWindow::AudioStopClicked(void)
{
    emit AudioStopClickedSignal();
    AudioRecordButton->setText(QString("Record Audio")) ;
    AudioPlayButton->setText(QString("Play Audio")) ;
}

void MainWindow::AudioPlayClicked(void)
{
#ifdef OS_WINDOWS
    AudioPlayfileName = QFileDialog::getOpenFileName(this, "Open Audio File", "", "*.mp3 *.wav");
#else
    AudioPlayfileName = "/usr/bin/audio_output.wav"; //44.1k_2ch_16b_1k_90.wav
#endif
    emit AudioPlayClickedSignal(&AudioPlayfileName);
}

void MainWindow::AudioRecordDurationUpdate(qint64 duration)
{
    //record time
    qDebug() << "AudioRecord:"<< duration;
    AudioRecordButton->setText(QString("Recorded %1 seconds").arg(duration / 1000) ) ;
}

void MainWindow::AudioPlayStatusUpdate(qint8 newState)
{
    //
    qDebug() << "AudioPlayStatus:"<< newState;
    if(newState == 1) // 1:QMediaPlayer::PlayingState
        AudioPlayButton->setText(QString("Playing"));
}

void MainWindow::AudioPlayMediaStatusUpdate(qint8 newState)
{
    //record time
    qDebug() << "AudioPlayMediaStatus:"<< newState;
    if(newState == 7)  // 7: QMediaPlayer::EndOfMedia
        AudioPlayButton->setText(QString("PlayDone")) ;
}


void MainWindow::DrawAudioPage(void)
{
    QGroupBox *AudioGroupBox = new QGroupBox("Audio",this);
    AudioGroupBox->setGeometry(1030,500,200,200);

    Audiolayout = new QVBoxLayout(this);

    //AddRecord
    AudioRecordButton = new QPushButton("Record Audio", this);
    Audiolayout->addWidget(AudioRecordButton);

    //add stopRecord
    stopRecordButton = new QPushButton("Stop Recording", this);
    Audiolayout->addWidget(stopRecordButton);

    //AddPlayButton
    AudioPlayButton = new QPushButton("Play Audio", this);
    Audiolayout->addWidget(AudioPlayButton);

    AudioGroupBox->setLayout(Audiolayout);
}



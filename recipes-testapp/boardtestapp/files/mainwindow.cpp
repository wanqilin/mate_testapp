#include "mainwindow.h"
#include "OpenCVWindow.h"
#include "osdupdatethread.h"
#include "opencvfacerecognition.h"
#include <QTimer>
#include <QtDebug>
#include <QDateTime>
#include <QPalette>
#include <QVBoxLayout>

#include <QProcess>
#include <QGroupBox>

using namespace std;

#ifdef OS_WINDOWS
// define GUID
DEFINE_GUID(GUID_DEVINTERFACE_USB_DEVICE,
            0xA5DCBF10L, 0x6530, 0x11D2, 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED);

// define GUID_DEVINTERFACE_DISK check usb disk
DEFINE_GUID(GUID_DEVINTERFACE_DISK,
            0x53f56307,0xb6bf,0x11d0,0x94,0xf2,0x00,0xa0,0xc9,0x1e,0xfb,0x8b);
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    //, pOpenCVWindow(nullptr)
{
    this->setWindowTitle(tr("NxpTestApp"));
    this->setGeometry(0,0,APP_WIDTH,APP_HEIGH);
    InitVariable();

    //SetTimer
    m_timer = new QTimer(this);
    m_timer->setSingleShot(false);  //Non-single trigger
    m_timer->setInterval( 1*1000 );
    connect(m_timer, SIGNAL(timeout()), this, SLOT(TimerHandle()));
    m_timer->start();


    osdupdatethread = new OSDUpdateThread();
    qthread = new QThread(this);
    osdupdatethread->moveToThread(qthread);
    qthread->start();

    SetSignalAndSLot();
    DrawOSDInterface();
}

MainWindow::~MainWindow()
{    
    //processor->requestInterruption();
    //processor->wait();
    //qthread->requestInterruption();
    //qthread->->wait();
}

void MainWindow::InitVariable(void)
{
    blanstatus = false;
}

void MainWindow::DrawOSDInterface(void)
{
    appbox = new QGroupBox(this);
    appbox->setGeometry(0,0,APP_WIDTH,APP_HEIGH);
    applayout = new QBoxLayout(QBoxLayout::TopToBottom,this);


    this->displayTitle = new QLabel("TestApp",this);
    this->displayTitle->setGeometry(560,0,200,50);
    this->displayTitle->setFont(QFont("Arial",14,QFont::Bold));
    //applayout->addWidget(displayTitle);
    //appbox->setLayout(applayout);

    //Draw Clock
    DrawClockPage();

    //draw wifi
    DrawWifiPage();

    //Draw Listen event
    DrawListenEventPage();

    //draw camera
    DrawCameraPage();

    emit StartOSDThread();
}

void MainWindow::SetSignalAndSLot(void)
{
    connect(this,&MainWindow::StartOpenCVfaceRecognition,this,&MainWindow::OpenCVfaceRecognitionHandle);
    connect(qthread, &QThread::finished, qthread, &QThread::deleteLater);
    connect(this, &MainWindow::StartOSDThread, osdupdatethread, &OSDUpdateThread::working);
    connect(osdupdatethread, &OSDUpdateThread::ReDrawOSD,this,&MainWindow::OSDUpdate);

    //listen network connect status
    //networkManager = new QNetworkConfigurationManager(this);
    //connect(networkManager, &QNetworkConfigurationManager::onlineStateChanged, this, &MainWindow::DrawlanStatusUpdate);
}

void MainWindow::PrintText(const QString &text)
{
    qDebug()<<text;
}

void MainWindow::TimerHandle(void)
{    
    ClockUpdate();
}

void MainWindow::OSDUpdate(void)
{
    wifiListUpdate();
#ifdef OS_UNIX
    USBDeviceUpdate();
#endif
}

void MainWindow::DrawClockPage(void)
{
    //draw lcdnumber
    this->lcdnumber = new QLCDNumber(19,this);
    this->lcdnumber->setSegmentStyle(QLCDNumber::Flat);
    this->lcdnumber->setGeometry(550,60,180,50);
    //applayout->addWidget(lcdnumber);
    //appbox->setLayout(applayout);
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
    QGroupBox *CameraGroupBox = new QGroupBox("Camera",this);
    CameraGroupBox->setGeometry(940,0,350,600);
    QVBoxLayout *Cameralayout = new QVBoxLayout(this);

    QPalette palette;
    palette.setColor(QPalette::Window,QColor(50, 50, 50));
    palette.setColor(QPalette::WindowText, Qt::white);
    //Camera View
    CameraView = new QLabel("No Data!",this);
    CameraView->setPalette(palette);
    CameraView->setFixedSize(320,240);
    CameraView->setAutoFillBackground(true);
    CameraView->setPalette(palette);
    CameraView->setAlignment(Qt::AlignCenter);

    //MatchImage
    MatchImage = new QLabel("No Data!",this);
    MatchImage->setPalette(palette);
    MatchImage->setFixedSize(320,240);
    MatchImage->setAutoFillBackground(true);
    MatchImage->setPalette(palette);
    MatchImage->setAlignment(Qt::AlignCenter);

    //CaptureButton
    captureButton= new QPushButton("Capture", this);
    captureButton->resize(180,50);

   //OpenCVButton
    //OpenCVButton = new QPushButton("GotoOpenCV",this);
    //OpenCVButton->resize(180,50);
    //connect(OpenCVButton,&QPushButton::clicked,this,&MainWindow::GotoOpenCVWindow);

    Cameralayout->addWidget(CameraView);
    Cameralayout->addWidget(MatchImage);
    Cameralayout->addWidget(captureButton);
    //Cameralayout->addWidget(OpenCVButton);

    CameraGroupBox->setLayout(Cameralayout);

    //applayout->addWidget(CameraGroupBox);
    //appbox->setLayout(applayout);

    //startOpenCVfaceRecognition
    //CameraInit();
    emit StartOpenCVfaceRecognition();
}

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
        viewfinder = new QCameraViewfinder(this);
        viewfinder->setFixedSize(320,240);
        viewfinder->setAutoFillBackground(true);
        viewfinder->setPalette(palette);
        //Cameralayout->addWidget(viewfinder);
        //CameraGroupBox->setLayout(Cameralayout);
        
        QCameraViewfinderSettings viewfinderSettings;
        viewfinderSettings.setResolution(640,480);
        viewfinderSettings.setMaximumFrameRate(15.0);
        viewfinderSettings.setMaximumFrameRate(30.0);
        camera->setViewfinderSettings(viewfinderSettings);

        //camer connect Viewfinder
        camera->setViewfinder(viewfinder);

        //Creat ImageCapture Object
        imageCapture = new QCameraImageCapture(camera, this);
        connect(captureButton,&QPushButton::clicked,this,&MainWindow::captureImage);
        connect(imageCapture, &QCameraImageCapture::imageCaptured, this, &MainWindow::displayImage);
        camera->start();
    }
    else
    {
        qDebug()<<"No camera available.";
        CameraView->setText("No camera available.");
    }
}

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

void MainWindow::OpenCVfaceRecognitionHandle(void)
{
    qDebug()<<"OpenCVfaceRecognitionHandle!";
    processor = new OpenCVfaceRecognition();
    connect(processor,&OpenCVfaceRecognition::frameProcessed,this,&MainWindow::displayImage);
    processor->start();
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
    QVBoxLayout *layout = new QVBoxLayout(this);
    listWidget = new QListWidget(this);
    listWidget->setFixedSize(150, 200);
    listWidget->setMinimumSize(150, 200);
    listWidget->setMaximumSize(200, 220);
    listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    layout->addWidget(listWidget);
    wifiGroupBox->setLayout(layout);
    //applayout->addWidget(wifiGroupBox);
    //appbox->setLayout(applayout);

}

void MainWindow::wifiListUpdate(void)
{
    this->listWidget->clear();
    //qDebug()<<"wifi list update!";
    // get wifi list
    QStringList wifiList = getWifiList();
    for (const QString &wifi : wifiList) {
        //qDebug()<<wifi;
        this->listWidget->addItem(wifi);
    }
}

QStringList  MainWindow::getWifiList(void)
{
    QStringList wifiList;
    QProcess process;
#ifdef OS_WINDOWS
    process.start("netsh", QStringList() << "wlan" << "show" << "network");
    process.waitForFinished();

    QString output = process.readAllStandardOutput();
    QStringList lines = output.split('\n');

    for (const QString &line : lines) {
        if (line.contains("SSID")) {
            QStringList parts = line.split(':');
            if (parts.size() > 1) {
                wifiList.append(parts[1].trimmed());
            }
        }
    }
#else
    process.start("nmcli", QStringList() << "d" << "wifi" << "list");
    process.waitForFinished();
    QTextStream stream(process.readAllStandardOutput());

    while (!stream.atEnd()) {
        QString line = stream.readLine();
        
        if (line.startsWith("SSID")) continue; // skip table

        QStringList fields = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        //qDebug()<<fields;
        if (fields.size() > 0) {
            wifiList.append(fields[1]); // add SSID
        }
    }
#endif

    return wifiList;
}

void MainWindow::DrawListenEventPage(void)
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
    usbtxt = new QLabel("USB-1",this);
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

    qDebug()<<"Usb Init Cnt:"<<getUSBDeviceCount();
    if(getUSBDeviceCount()>0)
        usbstatus->setStyleSheet("color: white; background-color: green; border-radius: 10px;");
    else
        usbstatus->setStyleSheet("color: black; background-color: gray; border-radius: 10px;");
    usbstatus->setNum(getUSBDeviceCount());
    //blanstatus = networkManager->isOnline();
    //DrawlanStatusUpdate(blanstatus);

}

void MainWindow::DrawlanStatusUpdate(bool isOnline)
{
    if (isOnline) {
        qDebug() << "Network connected!";
        lanstatus->setStyleSheet("background-color: green; border-radius: 10px;");
    } else {
        qDebug() << "Network disconnected!";
        lanstatus->setStyleSheet("background-color: gray; border-radius: 10px;");
    }
}

#ifdef OS_WINDOWS
bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result){
    int usbCnt=0;

    if (eventType == "windows_generic_MSG") {
        MSG *msg = static_cast<MSG *>(message);
        if (msg->message == WM_DEVICECHANGE) {
            if (msg->wParam == DBT_DEVICEARRIVAL) {
                qDebug() << "USB plug-in";
            } else if (msg->wParam == DBT_DEVICEREMOVECOMPLETE) {    
                qDebug() << "USB plug-out";
            }

            usbCnt=getUSBDeviceCount();
            qDebug()<<"Usb Cnt:"<<usbCnt;
            if(usbCnt>0)
                usbstatus->setStyleSheet("color: white; background-color: green; border-radius: 10px;");
            else
                usbstatus->setStyleSheet("color: black; background-color: gray; border-radius: 10px;");
            usbstatus->setNum(usbCnt);

        }
    }
    return QWidget::nativeEvent(eventType, message, result);
}

int MainWindow::getUSBDeviceCount() {
    // Get usb device info 
    HDEVINFO deviceInfoSet = SetupDiGetClassDevs(
         &GUID_DEVINTERFACE_DISK, // &GUID_DEVINTERFACE_USB_DEVICE,
        nullptr,
        nullptr,
        DIGCF_PRESENT | DIGCF_DEVICEINTERFACE
        );

    if (deviceInfoSet == INVALID_HANDLE_VALUE) {
        qWarning() << "GetDeviceInfo fail";
        return -1;
    }

    // loop device info
    int deviceCount = 0;
    SP_DEVINFO_DATA deviceInfoData;
    deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    for (int i = 0; SetupDiEnumDeviceInfo(deviceInfoSet, i, &deviceInfoData); i++) {
        deviceCount++;
    }

    // clean device info
    SetupDiDestroyDeviceInfoList(deviceInfoSet);
    return deviceCount;
}
#endif 
#ifdef OS_UNIX
bool MainWindow::isUsbStorage(const std::string& devicePath) {
    std::string usbPath = "/sys/class/block/" + devicePath + "/device";
    DIR* dir = opendir(usbPath.c_str());
    if (dir) {
        // If the device directory exists, it is a USB device
        closedir(dir);
        return true;
    }
    return false;
}

int MainWindow::getUSBDeviceCount() {
    int usbCount = 0;

    DIR* dir = opendir("/sys/class/block");
    if (!dir) {
        qDebug() << "Failed to open /sys/class/block";
        return -1;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        // exclude "." and ".."
        if (entry->d_name[0] == '.')
            continue;

        std::string deviceName(entry->d_name);

        if (isUsbStorage(deviceName)) {
            usbCount++;
            //qDebug() << "Found USB storage: " << deviceName.c_str();
        }
    }

    closedir(dir);
    return usbCount;
}

void MainWindow::USBDeviceUpdate(void)
{
    int usbCnt=0;

    usbCnt=getUSBDeviceCount();
    //qDebug()<<"Usb Cnt:"<<usbCnt;
    if(usbCnt>0)
        usbstatus->setStyleSheet("color: white; background-color: green; border-radius: 10px;");
    else
        usbstatus->setStyleSheet("color: black; background-color: gray; border-radius: 10px;");
    usbstatus->setNum(usbCnt);
}
#endif

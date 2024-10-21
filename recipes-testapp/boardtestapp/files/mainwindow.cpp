#include "mainwindow.h"
#include <QTimer>
#include <QtDebug>
#include <QDateTime>
#include <QPalette>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->setWindowTitle(tr("NxpTestApp"));
    this->setGeometry(0,0,APP_WIDTH,APP_HEIGH);
    DrawOSDInterface();
    SetSignalAndSLot();
    CameraInit();
}

MainWindow::~MainWindow() {}

void MainWindow::CameraInit(void)
{
    const QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    if (!cameras.isEmpty())
    {
     qDebug()<<"Camera deviceName:";
     qDebug()<<cameras.first().deviceName();
    //Creat CameraObject
    camera = new QCamera(cameras.first(),this);

    //Creat Viewfinder
    viewfinder = new QCameraViewfinder(this);
    viewfinder->setGeometry(960,0,320,240);

    //camer connect Viewfinder
    camera->setViewfinder(viewfinder);

    //Creat CaptureImage object
    CameraImage = new QLabel("ImageDisplay",this);
    CameraImage->setGeometry(960,240,320,240);
    CameraImage->clear();
    QPalette palette;
    palette.setColor(QPalette::Window,QColor(50, 50, 50));
    CameraImage->setAutoFillBackground(true);
    CameraImage->setPalette(palette);
    CameraImage->setScaledContents(true);

    //Creat ImageCapture Object
    imageCapture = new QCameraImageCapture(camera, this);
    connect(captureButton,&QPushButton::clicked,this,&MainWindow::captureImage);
    connect(imageCapture, &QCameraImageCapture::imageCaptured, this, &MainWindow::displayImage);
    camera->start();
    }
    else
    {
        qDebug()<<"No camera available.";
        CameraImage = new QLabel("No camera available.",this);
        CameraImage->setGeometry(960,0,320,240);
        CameraImage->setAlignment(Qt::AlignCenter);
        QPalette palette;
        palette.setColor(QPalette::WindowText,Qt::white);
    }
}

void MainWindow::DrawOSDInterface(void)
{
    this->displayTitle = new QLabel("i.Mx8MP Test App",this);
    this->displayTitle->setGeometry(560,0,300,50);
    this->displayTitle->setFont(QFont("Arial",14,QFont::Bold));

    this->lcdnumber = new QLCDNumber(19,this);
    this->lcdnumber->setSegmentStyle(QLCDNumber::Flat);
    this->lcdnumber->setGeometry(550,60,180,50);

    //CaptureButton
    captureButton= new QPushButton("CaptureImage", this);
    captureButton->setGeometry(1000,500,180,50);

    //SetTimer
    m_timer = new QTimer(this);
    m_timer->setSingleShot(false);  //Non-single trigger
    m_timer->setInterval( 1*1000 );
    connect(m_timer, SIGNAL(timeout()), this, SLOT(RecvTimer()));
    m_timer->start();
}

void MainWindow::SetSignalAndSLot(void)
{
    ;
}

void MainWindow::PrintText(const QString &text)
{
    qDebug()<<text;
}

void MainWindow::RecvTimer(void)
{
    //Get Current Time
    QDateTime dt = QDateTime::currentDateTime();
    QString strTime = dt.toString("yyyy-MM-dd HH:mm:ss");
    this->lcdnumber->display(strTime);

}


void MainWindow::captureImage() {
    // CaptureImage
    imageCapture->capture();
}

void MainWindow::displayImage(int id, const QImage &preview)
{
    Q_UNUSED(id);
    CameraImage->setPixmap(QPixmap::fromImage(preview));
}

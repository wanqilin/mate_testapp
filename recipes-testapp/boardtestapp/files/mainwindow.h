#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLCDNumber>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMainWindow>
#include <QTimer>
#include <QThread>
#include <QCamera>
#include <QMessageBox>
#include <QCameraViewfinder>
#include <QCameraImageCapture>
#include <QPushButton>
#include <QCameraInfo>
#include <QGroupBox>
#include <QBoxLayout>
#include <opencv2/core/core.hpp>
#include <QFileDialog>
#include <QMediaPlayer>
#include "sw_app_config.h"
#include "wirelessdeviceworkthread.h"
#include "hotplugworkthread.h"
#include "opencvcamerathread.h"
#include "osdeventwork.h"

using namespace cv;
using namespace std;

//class OpenCVWindow;
class OpenCVCameraThread;
class WirelessDeviceWorkThread;
class HotPlugWorkThread;
class OsdEventWork;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    #define APP_WIDTH   1280
    #define APP_HEIGH   800

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void CameraInit();
signals:
    void AudioRecordClickedSignal(QString *sAudiofileName);
    void AudioStopClickedSignal(void);
    void AudioPlayClickedSignal(QString *sAudiofileName);
    void DemoAppLicLimit(void);

private slots:
    void PrintText(const QString &text);
    void captureImage();
    void displayImage(int id, const QImage &preview);
    //void GotoOpenCVWindow();
    void ClockUpdate(void);
    void DrawlanStatusUpdate(bool isOnline);
    void wifiListUpdate(const QStringList &wifiList);
    void UsbDeviceUpdate(int usbCnt);
    void AudioRecordDurationUpdate(qint64 duration);
    void AudioRecordClicked();
    void AudioStopClicked();
    void AudioPlayClicked();
    void AudioPlayStatusUpdate(qint8 newState);
    void AudioPlayMediaStatusUpdate(qint8 newState);
    void btListUpdate(const QStringList &btList);
#if(APP_LIC_TYPE==0)
    void AppLicLimitshowInfo();
#endif
    void AppExit();
private:
    qint8  AppLicCnt;
    QString AudioRecordfileName;
    QString AudioPlayfileName;
    QPushButton *captureButton;
    QPushButton *OpenCVButton;
    QPushButton *AudioPlayButton;
    QPushButton *AudioRecordButton;
    QPushButton *stopRecordButton;
    QLCDNumber *lcdnumber;
    QListWidget *WifilistWidget;
    QListWidget *BtlistWidget;
    QLabel *displayTitle;
    QTimer *m_timer;
    QTimer *AppExittimer;
    QCamera *camera;
    QCameraViewfinder *viewfinder;
    QCameraImageCapture *imageCapture;
    QVideoWidget *CameraViewVideoFrame;
    QMediaPlayer *mediaPlayer;    
    QLabel *CameraView;
    QLabel *MatchImage;
    QLabel *lanstatus;
    QLabel *usbtxt;
    QLabel *usbstatus;

    QGroupBox *appbox;
    QGroupBox *lanbox;
    QGroupBox *usbbox;
    QBoxLayout *applayout;
    QHBoxLayout *usblayout;
    QVBoxLayout *Audiolayout;
    //OpenCVWindow *pOpenCVWindow;
    OpenCVCameraThread *pOpenCVCameraThread;
    WirelessDeviceWorkThread *pWirelessDeviceWorkThread;
    HotPlugWorkThread *pHotPlugWorkThread;
    QThread *pOsdEventThread;
    OsdEventWork *pOsdEventWork;

    void DrawOSDInterface(void);
    void SetSignalAndSLot(void);
    QStringList getWifiList();
    void DrawClockPage();
    void DrawWifiPage();
    void DrawCameraPage();
    void InitVariable();
    void DrawAudioPage();
    void DrawBtPage();
    void DrawEventListenPage();
#if(APP_LIC_TYPE==0)
    void saveData(int value);
    int loadData();
#endif
    void AppLicCheck();
};
#endif // MAINWINDOW_H

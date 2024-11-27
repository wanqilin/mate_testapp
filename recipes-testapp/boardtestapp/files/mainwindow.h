#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "sw_app_config.h"
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
#include <QNetworkConfigurationManager>
#include <opencv2/core/core.hpp>
#ifdef OS_UNIX
#include <dirent.h>
#endif
#ifdef OS_WINDOWS
#include <windows.h>
#include <QDir>
#include <QFileInfo>
#include <dbt.h>
#include <SetupAPI.h>
#include <initguid.h>
#include <devguid.h>
#endif
#include "osdupdatethread.h"

using namespace cv;
using namespace std;

//class OpenCVWindow;
class OpenCVfaceRecognition;
class OSDUpdateThread;

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
    void StartOpenCVfaceRecognition(void);
    void StartOSDThread(void);

protected:
#ifdef OS_WINDOWS
    bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;
#endif

private slots:
    void PrintText(const QString &text);
    void captureImage();
    void displayImage(int id, const QImage &preview);
    //void GotoOpenCVWindow();
    void OpenCVfaceRecognitionHandle();
    void TimerHandle();
    void OSDUpdate();
    void DrawlanStatusUpdate(bool isOnline);
private:
    bool blanstatus;
    QPushButton *captureButton;
    QPushButton *OpenCVButton;
    QLCDNumber *lcdnumber;
    QListWidget *listWidget;
    QLabel *displayTitle;
    QTimer *m_timer;
    QCamera *camera;
    QCameraViewfinder *viewfinder;
    QCameraImageCapture *imageCapture;
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
    //OpenCVWindow *pOpenCVWindow;
    OpenCVfaceRecognition *processor;
    OSDUpdateThread* osdupdatethread;
    QThread* qthread;
    //QNetworkConfigurationManager *networkManager;

    void DrawOSDInterface(void);
    void SetSignalAndSLot(void);
    QStringList getWifiList();
    void wifiListUpdate();
    void ClockUpdate();
    void DrawClockPage();
    void DrawWifiPage();
    void DrawCameraPage();
    void DrawListenEventPage();
    void InitVariable();
    int getUSBDeviceCount();
#ifdef OS_UNIX
    bool isUsbStorage(const std::string &devicePath);
    void USBDeviceUpdate();
#endif
};
#endif // MAINWINDOW_H

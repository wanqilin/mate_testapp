#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLCDNumber>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QTimer>
#include <QMainWindow>
#include <QCamera>
#include <QCameraViewfinder>
#include <QCameraImageCapture>
#include <QPushButton>
#include <QCameraInfo>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    #define APP_WIDTH   1280
    #define APP_HEIGH   800

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void CameraInit();
public slots:
    void RecvTimer();

private slots:
    void PrintText(const QString &text);

    void captureImage();
    void displayImage(int id, const QImage &preview);
private:
    QPushButton *captureButton;
    QLCDNumber *lcdnumber;
    QLabel *displayTitle;
    QTimer *m_timer;
    QCamera *camera;
    QCameraViewfinder *viewfinder;
    QCameraImageCapture *imageCapture;
    QLabel *CameraImage;

    void DrawOSDInterface(void);
    void SetSignalAndSLot(void);
};
#endif // MAINWINDOW_H

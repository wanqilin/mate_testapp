#ifndef OPENCVCAMERATHREAD_H
#define OPENCVCAMERATHREAD_H

#include <QObject>
#include <QThread>
#include <QFile>
#include <opencv2/core/core.hpp>

using namespace std;
using namespace cv;


class OpenCVCameraThread : public QThread
{
    Q_OBJECT
public:
#ifdef OS_WINDOWS
    #define FRONTAL_FACE_FILE "D:/work/OpenCV/opencv-4.5.1/QT-opencv-4.5.1/source/data/haarcascades/haarcascade_frontalface_alt2.xml"
#else
   #define FRONTAL_FACE_FILE "/usr/share/opencv4/haarcascades/haarcascade_frontalface_alt2.xml"
#endif
    #define MY_FACE_STORE "./MyFaceStore.xml"
    OpenCVCameraThread(QObject *parent = nullptr);
    ~OpenCVCameraThread();
    void run() override;

    void stop();
signals:
    void frameProcessed(int id,const QImage &image);

private:
    bool stopRequested;
    QImage Mat2QImage(Mat cvImg);

};

#endif // OPENCVCAMERATHREAD_H

#ifndef OPENCVFACERECOGNITION_H
#define OPENCVFACERECOGNITION_H

#include <QObject>
#include <QThread>
#include <QFile>
#include <opencv2/core/core.hpp>

using namespace std;
using namespace cv;


class OpenCVfaceRecognition : public QThread
{
    Q_OBJECT
public:
#ifdef OS_WINDOWS
    #define FRONTAL_FACE_FILE "D:/work/OpenCV/opencv-4.5.1/QT-opencv-4.5.1/source/data/haarcascades/haarcascade_frontalface_alt2.xml"
#else
   #define FRONTAL_FACE_FILE "/usr/share/opencv4/haarcascades/haarcascade_frontalface_alt2.xml"
#endif
    #define MY_FACE_STORE "./MyFaceStore.xml"
    OpenCVfaceRecognition(QObject *parent = nullptr);
    ~OpenCVfaceRecognition();
    void run() override;

signals:
    void frameProcessed(int id,const QImage &image);

private:
    QImage Mat2QImage(Mat cvImg);

};

#endif // OPENCVFACERECOGNITION_H

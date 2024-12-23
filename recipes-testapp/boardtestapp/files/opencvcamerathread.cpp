#include "opencvcamerathread.h"
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
//#include <opencv2\imgproc\types_c.h>

#include <QImage>
#include <QtDebug>
#include <vector>


OpenCVCameraThread::OpenCVCameraThread(QObject *parent) : QThread(parent)
{
    stopRequested = false;
}

OpenCVCameraThread::~OpenCVCameraThread() 
{
    stopRequested = false;
}

void OpenCVCameraThread::stop()
{
    qDebug()<<"OpenCVCameraThread to stop!";
    stopRequested = true;
}

void OpenCVCameraThread::run()
{
    Mat src;
    Mat gray;
    Mat dst;

    qDebug()<<"OpenCVCameraThread Thread run!";

    CascadeClassifier c;
#ifdef OS_UNIX
    VideoCapture v(3);
#else
    VideoCapture v(0);
#endif

    if (!v.isOpened()) {
        return;
    }

    if(!c.load(FRONTAL_FACE_FILE))
    {
        qDebug()<<"Load haarcascade_frontalface_alt2 fail!";
        return;
    }

    vector<Rect> facesPos;


    while((!stopRequested)&&(v.read(src)))
    {
        /*
        flip(src, src, 1);
        cvtColor(src, gray, cv::COLOR_BGR2GRAY);
        equalizeHist(gray, dst);
        c.detectMultiScale(dst, facesPos);

        for(std::vector<cv::Rect>::size_type i=0; i<facesPos.size(); i++)
        {
            rectangle(src, facesPos[i], Scalar(0,0,255), 2);
        }
        */
        QImage qimg = Mat2QImage(src);
        emit frameProcessed(0x1,qimg);
        QThread::msleep(30);

        //imshow("Test1", src);
        //imshow("Test2", gray);
        //imshow("Test3", dst);

    }
}

QImage OpenCVCameraThread::Mat2QImage(Mat cvImg)
{
    QImage qImg;
    if(cvImg.channels()==3)     //3 channels color image
    {

        cv::cvtColor(cvImg,cvImg,cv::COLOR_BGR2RGB);
        qImg =QImage((const unsigned char*)(cvImg.data),
                      cvImg.cols, cvImg.rows,
                      cvImg.cols*cvImg.channels(),
                      QImage::Format_RGB888);
    }
    else if(cvImg.channels()==1)                    //grayscale image
    {
        qImg =QImage((const unsigned char*)(cvImg.data),
                      cvImg.cols,cvImg.rows,
                      cvImg.cols*cvImg.channels(),
                      QImage::Format_Indexed8);
    }
    else
    {
        qImg =QImage((const unsigned char*)(cvImg.data),
                      cvImg.cols,cvImg.rows,
                      cvImg.cols*cvImg.channels(),
                      QImage::Format_RGB888);
    }
    return qImg;
}

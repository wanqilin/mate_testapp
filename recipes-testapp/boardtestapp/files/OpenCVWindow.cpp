#include "OpenCVWindow.h"
#include <QtDebug>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2\imgproc\types_c.h>
#include <QImage>

using namespace std;
using namespace cv;

OpenCVWindow::OpenCVWindow(QWidget *parent)
    : QMainWindow(parent)
{
    qDebug()<<"IntoOpenCVWindow!";
    setWindowTitle(tr("OpenCV"));
    resize(500, 600);

    OpencvImage = new QLabel("ImageDisplay",this);
    OpencvImage->resize(500,600);

    faceRecognition();
#if 0
    string imgPath = "D:\\wql\\openCV\\testPIC\\OpenCV_alpha_1.png";
    Mat src = imread(imgPath, IMREAD_COLOR);
    //imshow("test",src);

    OpencvImage->clear();
    OpencvImage->setScaledContents(true);
    QImage qimg = Mat2QImage(src);
    OpencvImage->setPixmap(QPixmap::fromImage(qimg));
#endif
}

OpenCVWindow::~OpenCVWindow() {}

int OpenCVWindow::faceRecognition(void)
{
    //1、定义视频流对象
    VideoCapture v(0);             //表明使用摄像头构造一个视频流对象

    //2、读取摄像头中的图像
    Mat src;                   //用于存放读取出来的图像
    Mat gray;                   //用于存储灰度图的图像容器
    Mat dst;                     //用于存储均衡化处理后的图像容器

    //5、实例化一个级联分类器的对象，用于找到图像中的人脸矩形区域
    CascadeClassifier c;
    //给类对象装载人脸识别模型
    //函数原型：bool load( const String& filename );
    //功能：给级联分类器对象，下载一个识别模型
    //参数：人脸识别模型的文件路径
    //返回值：成功下载返回真，失败返回假
    if(!c.load("D:/wql/openCV/4.5.1/QT-opencv-4.5.1/source/data/haarcascades/haarcascade_frontalface_alt2.xml"))
    {
        //QMessageBox::information(NULL,"失败", "人脸识别模型装载失败");
        qDebug()<<"Load haarcascade_frontalface_alt2 fail!";
        return -1;
    }

    //定义容器存放人脸分类后的矩形框
    vector<Rect> faces;

    //函数原型：virtual bool read(OutputArray image);
    //功能：从视频流中读取一张图像放入参数中
    //参数：图像容器
    //返回值：成功返回真，失败或者读取视频结束返回假
    while(v.read(src))
    {
        //将图像进行翻转
        //函数原型：void flip(InputArray src, OutputArray dst, int flipCode);
        //参数1：要翻转的图像
        //参数2：翻转后的图像容器
        //参数3：翻转规则：正数表示按y轴翻转，0表示按x轴翻转，负数表示按xy轴翻转
        flip(src, src, 1);

        cvtColor(src, gray, CV_BGR2GRAY);
        equalizeHist(gray, dst);
        c.detectMultiScale(dst, faces);

        //7、将上述得到的矩形框，全部都绘制到图像上
        for(std::vector<cv::Rect>::size_type i=0; i<faces.size(); i++)
        {
            //将任意一个矩形框，全部都绘制到图像上
            //函数原型：void rectangle(CV_IN_OUT Mat& img, Rect rec,const Scalar& color, int thickness = 1）
            //参数1：要被绘制的图像
            //参数2：要绘制的矩形框
            //参数3：矩形框的颜色
            //参数4：矩形框的粗细
            //返回值：无
            rectangle(src, faces[i], Scalar(0,0,255), 2);
        }

        //展示图像
        imshow("Test1", src);
        imshow("Test2", gray);
        imshow("Test3", dst);


        //加延时函数
        //函数原型：int waitKey(int delay = 0);
        //参数：等待时间
        //返回值：在等待期间用户按下的键盘的ascii值    ESC键对应的值为27
        if(waitKey(20)==27)
        {
            break;
        }
    }
    return 0;
}

QImage OpenCVWindow::Mat2QImage(Mat cvImg)
{
    QImage qImg;
    if(cvImg.channels()==3)     //3 channels color image
    {

        cv::cvtColor(cvImg,cvImg,CV_BGR2RGB);
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

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
    //1��������Ƶ������
    VideoCapture v(0);             //����ʹ������ͷ����һ����Ƶ������

    //2����ȡ����ͷ�е�ͼ��
    Mat src;                   //���ڴ�Ŷ�ȡ������ͼ��
    Mat gray;                   //���ڴ洢�Ҷ�ͼ��ͼ������
    Mat dst;                     //���ڴ洢���⻯������ͼ������

    //5��ʵ����һ�������������Ķ��������ҵ�ͼ���е�������������
    CascadeClassifier c;
    //�������װ������ʶ��ģ��
    //����ԭ�ͣ�bool load( const String& filename );
    //���ܣ���������������������һ��ʶ��ģ��
    //����������ʶ��ģ�͵��ļ�·��
    //����ֵ���ɹ����ط����棬ʧ�ܷ��ؼ�
    if(!c.load("D:/wql/openCV/4.5.1/QT-opencv-4.5.1/source/data/haarcascades/haarcascade_frontalface_alt2.xml"))
    {
        //QMessageBox::information(NULL,"ʧ��", "����ʶ��ģ��װ��ʧ��");
        qDebug()<<"Load haarcascade_frontalface_alt2 fail!";
        return -1;
    }

    //��������������������ľ��ο�
    vector<Rect> faces;

    //����ԭ�ͣ�virtual bool read(OutputArray image);
    //���ܣ�����Ƶ���ж�ȡһ��ͼ����������
    //������ͼ������
    //����ֵ���ɹ������棬ʧ�ܻ��߶�ȡ��Ƶ�������ؼ�
    while(v.read(src))
    {
        //��ͼ����з�ת
        //����ԭ�ͣ�void flip(InputArray src, OutputArray dst, int flipCode);
        //����1��Ҫ��ת��ͼ��
        //����2����ת���ͼ������
        //����3����ת����������ʾ��y�ᷭת��0��ʾ��x�ᷭת��������ʾ��xy�ᷭת
        flip(src, src, 1);

        cvtColor(src, gray, CV_BGR2GRAY);
        equalizeHist(gray, dst);
        c.detectMultiScale(dst, faces);

        //7���������õ��ľ��ο�ȫ�������Ƶ�ͼ����
        for(std::vector<cv::Rect>::size_type i=0; i<faces.size(); i++)
        {
            //������һ�����ο�ȫ�������Ƶ�ͼ����
            //����ԭ�ͣ�void rectangle(CV_IN_OUT Mat& img, Rect rec,const Scalar& color, int thickness = 1��
            //����1��Ҫ�����Ƶ�ͼ��
            //����2��Ҫ���Ƶľ��ο�
            //����3�����ο����ɫ
            //����4�����ο�Ĵ�ϸ
            //����ֵ����
            rectangle(src, faces[i], Scalar(0,0,255), 2);
        }

        //չʾͼ��
        imshow("Test1", src);
        imshow("Test2", gray);
        imshow("Test3", dst);


        //����ʱ����
        //����ԭ�ͣ�int waitKey(int delay = 0);
        //�������ȴ�ʱ��
        //����ֵ���ڵȴ��ڼ��û����µļ��̵�asciiֵ    ESC����Ӧ��ֵΪ27
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

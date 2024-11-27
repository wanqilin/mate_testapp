#ifndef OPENCVWINDOW_H
#define OPENCVWINDOW_H
#include <QMainWindow>
#include <QLabel>
#include <opencv2/core/core.hpp>


using namespace cv;
using namespace std;

class OpenCVWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit OpenCVWindow(QWidget *parent = nullptr);
    ~OpenCVWindow();

private:
    QLabel *OpencvImage;
    QImage Mat2QImage(Mat cvImg);
    int faceRecognition(void);
};

#endif // OPENCVWINDOW_H

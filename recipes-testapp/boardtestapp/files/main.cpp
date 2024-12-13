#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qDebug()<<"Hello NxpTestApp Qt Version" << QT_VERSION_STR;

    //Creat font object,Set font and size
    QFont font("Arial",10);
    //set app global font
    a.setFont(font);

    MainWindow w;
    w.show();
    return a.exec();
}
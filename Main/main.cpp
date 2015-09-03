#include <QApplication>
#include "GUI/Application/MainApplication.h"
#include <GUI/MainWindow/MainWindow.h>

int main(int argc, char *argv[])
{
//#ifdef USE_FREEGLUT
//    glutInit(&argc,argv);
//#endif
    QApplication a(argc, argv);
    GCL::GUI::MainApplication mainApp;
    QWidget* mainwindow = (new GCL::GUI::MainWindow(mainApp));
    mainwindow->show();
    mainwindow->setGeometry(100,100,1000,600);

    return a.exec();
}

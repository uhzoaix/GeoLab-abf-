#include <QApplication>
#include "TestOpenMeshAlgorithms/test_triangulation/TestTriangulationWidget.h"
using namespace GCL;
int main(int argc, char *argv[])
{
//#ifdef USE_FREEGLUT
//    glutInit(&argc,argv);
//#endif
    QApplication a(argc, argv);
    QWidget* mainwindow = (new GCL::Test::TestTriangulationWidget());
    mainwindow->show();
    mainwindow->setGeometry(100,100,1000,600);

    return a.exec();
}

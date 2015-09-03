#include <QApplication>
#include <GCL/GUI/Widgets/RenderWidget/RenderWidget.h>
#include <memory>
#include <GCL/Core/SceneGraph/SceneGraph.h>
#include <GCL/Core/DataStructures/OMesh.h>
#include <GCL/Core/SceneGraph/OpenMeshNode.h>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <GCL/Core/SceneGraph/NodeFactory.h>
#include <GL/glut.h>
#include "mainwindow.h"
int main(int argc, char *argv[])
{
//#ifdef USE_FREEGLUT
//    glutInit(&argc,argv);
//#endif
    QApplication a(argc,argv);
    mainwindow mainwindow_;
    mainwindow_.show();
    mainwindow_.resize(1000,600);





    return a.exec();
}





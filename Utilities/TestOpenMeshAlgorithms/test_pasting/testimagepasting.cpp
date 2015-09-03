
#include <GCL/Core/SceneGraph/SceneGraph.h>
#include <GCL/GUI/Widgets/RenderWidget/RenderWidget.h>
#include "ImagePastingNode.h"
#include <GCL/Core/DataStructures/OMesh.h>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/IO/IOInstances.hh>
#include <QApplication>
#include <GCL/Core/SceneGraph/OpenMeshNode.h>
#include <GL/glut.h>
#include "mainwindow.h"
int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    mainwindow mainwindow_;
    mainwindow_.show();
    mainwindow_.setGeometry(100,100,1000,600);

    return a.exec();
}

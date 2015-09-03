#include "mainwindow.h"
#include <GCL/GUI/Widgets/RenderWidget/RenderWidget.h>
#include <memory>
#include <GCL/Core/SceneGraph/SceneGraph.h>
#include <GCL/Core/DataStructures/OMesh.h>
#include <GCL/Core/SceneGraph/OpenMeshNode.h>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <GCL/Core/SceneGraph/NodeFactory.h>
#include <QToolBar>
#include <QAction>
#include <GCL/Core/gclnamespace.h>
#include <GCL/Core/SceneGraph/RenderModes.h>
#include <ctime>
#include "ImagePastingNode.h"
#include <Utilities/OpenMeshAlgorithms/Pasting/ImagePasting.h>
mainwindow::mainwindow(QWidget *parent) : QMainWindow(parent)
{
    scenegraph_ = new GCL::SceneGraph::SceneGraphNode();
    GCL::GUI::RenderWidget* render_widget = new GCL::GUI::RenderWidget(scenegraph_,this);
    this->setCentralWidget(render_widget);

    QToolBar* toolbar = this->addToolBar("main");


    QAction* pAction = new QAction("Init",this);
    toolbar->addAction(pAction);
    connect(pAction,SIGNAL(triggered()),this,SLOT(init()));

    pAction = new QAction("Compute",this);
    toolbar->addAction(pAction);
    connect(pAction,SIGNAL(triggered()),this,SLOT(compute()));

    pAction = new QAction("Check_Circle",this);
    toolbar->addAction(pAction);
    connect(pAction,SIGNAL(triggered()),this,SLOT(check_circle()));
}

mainwindow::~mainwindow()
{
    if(scenegraph_)
    {
        delete scenegraph_;
        scenegraph_ = NULL;
    }
}

void mainwindow::init()
{
    std::shared_ptr<GCL::OMesh> mesh(new GCL::OMesh());

   OpenMesh::IO::read_mesh(*mesh,"mtfy_init.stl");
    mesh->MeshNormalize();
    mesh->update_normals();

    GCL::SceneGraph::ImagePastingNode* image_pasting = (new GCL::SceneGraph::ImagePastingNode(mesh,scenegraph_));
    image_pasting->getImagePasting()->setMesh(mesh);
    scenegraph_->nodeUpdated();
}

void mainwindow::compute()
{



}

void mainwindow::check_circle()
{



}



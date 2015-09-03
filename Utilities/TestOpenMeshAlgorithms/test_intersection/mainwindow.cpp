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
#include <Utilities/OpenMeshAlgorithms/Geometry/NearestTriangleSearch/NearestTrianglesQuery.h>
#include <GCL/Core/SceneGraph/RenderModes.h>
#include "MeshIntersections.h"
#include <ctime>
mainwindow::mainwindow(QWidget *parent) : QMainWindow(parent),mesh0_(NULL),mesh1_(NULL)
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
    GCL::SceneGraph::OpenMeshNode* node = scenegraph_->getNodeFactory()->createOpenMeshNode("mtfy_init.stl",scenegraph_);
    mesh0_ = node->getMesh().get();
    node->setRenderSettingsOverriding(true);
    node->getRenderModes()->set_render_mode_directly( GCL::SceneGraph::RenderModes::FACE_DRAWING
                                                  );

    node = scenegraph_->getNodeFactory()->createOpenMeshNode("cow.stl",scenegraph_);
    mesh1_ = node->getMesh().get();
    for(int i=0; i < (int)mesh1_->getPointsNumber(); i++)
    {
        mesh1_->setVertexPosition(i,mesh1_->getVertexPosition(i) + GCL::Vec3(0,0,0.01));
    }

    scenegraph_->nodeUpdated();
}

void mainwindow::compute()
{
    if(!mesh0_ || !mesh1_) return;
    GCL::Utilities::MeshIntersections meshintersections;
    meshintersections.compute_intersections(mesh0_,mesh1_);
    scenegraph_->getFirstDescendant<GCL::SceneGraph::OpenMeshNode* >()->nodeUpdated(GCL::Color_Dirty);


}

void mainwindow::check_circle()
{
    for(size_t i=0; i < mesh0_->getFacesNumber(); i++)
    {
        GCL::OMesh::FaceHandle fh = mesh0_->face_handle_(i);
        if(!mesh0_->isFaceSelected(i)) continue;
        int count = 0;
        for(GCL::OMesh::FaceFaceIter itr = mesh0_->ff_iter_(fh); itr.is_valid(); itr++)
        {
            if(mesh0_->status_(*itr).selected())
            {
                count++;
            }
        }
        std::cout<<count<<std::endl;
    }


}



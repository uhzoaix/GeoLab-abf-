#include "RenderWidget.h"

#include "GUI/Application/MainApplication.h"
#include <Core/SceneGraph/SceneGraph.h>
#include <Core/Utilities/LogSystem.h>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QWheelEvent>

namespace GCL {namespace GUI{

RenderWidget::RenderWidget(SceneGraph::SceneGraphNode *_scenegraph, QWidget *parent)
    :scenegraph_(_scenegraph), QGLWidget(parent)
{
    connect(scenegraph_,SIGNAL(onUpdate()),this,SLOT(updateGL()));
}

RenderWidget::~RenderWidget()
{
//    DebugLog<<"~RenderWidget()"<<std::endl;
}

void RenderWidget::initializeGL()
{
    glClearColor(1.0,1.0,1.0,1.0);
}

void RenderWidget::resizeGL(int w, int h)
{
    scenegraph_->resize(w,h);
}

void RenderWidget::paintGL()
{
   renderVisitor_.render(scenegraph_);
}

void RenderWidget::mousePressEvent(QMouseEvent *e)
{
    /// Screened Corrdinate System in OpenGL  start from left-bottom, is diffrent with  Qt's,
    /// Turn it into OpenGL screen corrdiantes;
    scenegraph_->mousePress(e->x(),height() - e->y(),e->button(),e->modifiers());
    updateGL();
}

void RenderWidget::mouseReleaseEvent(QMouseEvent *e)
{
    scenegraph_->mouseRelease(e->x(),height() - e->y(),e->button(),e->modifiers());
    updateGL();
}

void RenderWidget::mouseMoveEvent(QMouseEvent *e)
{
    scenegraph_->mouseMove(e->x(),height() - e->y());
    updateGL();
}

void RenderWidget::keyPressEvent(QKeyEvent *e)
{
    scenegraph_->keyDown(e->key(),e->modifiers());
    updateGL();
}

void RenderWidget::keyReleaseEvent(QKeyEvent *e)
{
    scenegraph_->keyUp(e->key(),e->modifiers());
    updateGL();
}

void RenderWidget::wheelEvent(QWheelEvent *e)
{
    scenegraph_->wheel(e->delta());
    updateGL();
}

void RenderWidget::timerEvent(QTimerEvent *)
{
    updateGL();
}

void RenderWidget::hideEvent(QHideEvent *)
{
    scenegraph_->setVisible(false);
}

void RenderWidget::showEvent(QShowEvent *)
{
    scenegraph_->setVisible(true);
    updateGL();
}



}}

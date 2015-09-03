#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H
#include <QGLWidget>
#include <Core/SceneGraph/RenderVisitor.h>
namespace GCL {
namespace SceneGraph {
class SceneGraphNode;}

namespace GUI{
class MainApplication;
class GCL_GUI_SHARED_EXPORT RenderWidget : public QGLWidget
{
        Q_OBJECT
public:
    RenderWidget(SceneGraph::SceneGraphNode* _scenegraph, QWidget* parent = 0);
    ~RenderWidget();
public:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent *e);
    void timerEvent(QTimerEvent *);
    void hideEvent(QHideEvent *);
    void showEvent(QShowEvent *);

    /// It wouldn't not be called in GUI app;
    /// The keyUp & KeyDown Of SceneGraph will be handled in SceneGraphWidget
    /// We still write these two function here for using this class independently without GUI app;
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
private:
    SceneGraph::SceneGraphNode* scenegraph_;
    SceneGraph::RenderVisitor renderVisitor_;

};
}}
#endif // RENDERWIDGET_H

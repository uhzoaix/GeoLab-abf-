#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
namespace GCL {
class OMesh;
namespace SceneGraph {
class SceneGraphNode;
}}
class mainwindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit mainwindow(QWidget *parent = 0);
    ~mainwindow();

signals:

public slots:


    void init();

    void compute();

    void check_circle();
private:
    GCL::SceneGraph::SceneGraphNode* scenegraph_;
    GCL::OMesh* mesh0_;
    GCL::OMesh* mesh1_;
};

#endif // MAINWINDOW_H

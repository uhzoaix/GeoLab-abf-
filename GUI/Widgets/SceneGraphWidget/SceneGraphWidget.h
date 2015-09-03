#ifndef GCL_GUI_SCENEGRAPHWIDGET_H
#define GCL_GUI_SCENEGRAPHWIDGET_H
#include <Core/gcl_global.h>
#include <QTreeView>
#include <memory>

namespace GCL { namespace SceneGraph {
class SceneGraphNode;}}


namespace GCL { namespace GUI {
class SceneGraphItemModel;
class GCL_GUI_SHARED_EXPORT SceneGraphWidget : public QTreeView
{
    Q_OBJECT
public:
    explicit SceneGraphWidget(SceneGraph::SceneGraphNode* _scenegraph,QWidget *parent = 0);

    //// events
    virtual void contextMenuEvent(QContextMenuEvent* _event);
    virtual void keyReleaseEvent(QKeyEvent* _event);

public slots:
    void onUpdate();
  private slots:
    void slotDoubleClicked(const QModelIndex& _index);
    void slotModelAboutToBeReset();
    void slotModelReset();
private:
    std::shared_ptr<SceneGraphItemModel> SceneGraphItemModel_;
    QModelIndex selectedIndex_;
    SceneGraph::SceneGraphNode* scenegraph_;


};
}}
#endif // SCENEGRAPHWIDGET_H

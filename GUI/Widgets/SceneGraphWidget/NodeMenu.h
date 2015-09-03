#ifndef GCL_GUI_NODEMENU_H
#define GCL_GUI_NODEMENU_H
#include <Core/gcl_global.h>
#include <QMenu>
class QAction;
class QActionGroup;
namespace GCL { namespace SceneGraph {
class Node;
class NodeAction;
}}
using namespace GCL::SceneGraph;
namespace GCL { namespace GUI {

class GCL_GUI_SHARED_EXPORT QNodeAction : public QAction
{
    Q_OBJECT
public:
    QNodeAction(NodeAction* node_action, Node* node, QObject* parent);
private slots:
    void slotNodeAction();
    void slotNodeAction(bool checked);
private:
    Node* node_;
};

class NodeMenu : public QMenu {

    Q_OBJECT

public:
    explicit NodeMenu(Node * _node, QWidget* _parent);
    virtual ~NodeMenu();

private:
    void createActionsAndMenus();//convenience function

private slots:





private:


    Node* node_;

};
}}
#endif // NODEMENU_H

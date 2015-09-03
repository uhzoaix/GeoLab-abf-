#include "NodeMenu.h"
#include <Core/SceneGraph/NodeType.h>
#include <Core/SceneGraph/RenderModes.h>
#include <Core/SceneGraph/SceneGraph.h>
#include <Core/SceneGraph/NodeAction.h>

#include <QMap>
namespace GCL { namespace GUI {

QNodeAction::QNodeAction(NodeAction *node_action, Node *node, QObject *parent)
    :node_(node),QAction(parent)
{
    this->setObjectName(node_action->getName());
    this->setText(node_action->getLabel());
    this->setToolTip(node_action->getToolTip());
    this->setCheckable(node_action->isCheckable());
    if(node_ && this->isCheckable())
    {
        this->setChecked(node_->getNodeActionValue(node_action->getName()));
    }
    connect(this,SIGNAL(triggered()),this,SLOT(slotNodeAction()));
    connect(this,SIGNAL(toggled(bool)),this,SLOT(slotNodeAction(bool)));
}

void QNodeAction::slotNodeAction()
{
    if(node_)
    {
        node_->slotAction(this->objectName());
    }
}

void QNodeAction::slotNodeAction(bool checked)
{
    if(node_)
    {
        node_->slotCheckableAction(this->objectName(),checked);
    }
}

NodeMenu::NodeMenu(Node *_node, QWidget *_parent)
    :node_(_node),QMenu(_parent)
{
    createActionsAndMenus();
}

NodeMenu::~NodeMenu()
{

}
void NodeMenu::createActionsAndMenus()
{
    if(!node_) return;
    QMap<QString,QMenu* > menuMap;

    for(int i=0; i < node_->getNodeActionsNumber(); i++)
    {
        QMenu* menu = this;
        QString category = node_->getNodeAction(i)->getCategory();
        if(category != "")
        {
            if(menuMap.count(category))
            {
                menu = menuMap.value(category);
            }
            else
            {
                menu = new QMenu(category,this);
                this->addMenu(menu);
                menuMap.insert(category,menu);
            }
        }
        menu->addAction(new QNodeAction(node_->getNodeAction(i),node_,this));
    }
}



}}

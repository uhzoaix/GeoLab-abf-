#include "SceneGraphWidget.h"
#include "SceneGraphItemModel.h"
#include <Core/SceneGraph/SceneGraph.h>
#include <Core/SceneGraph/RenderModes.h>
#include <Core/SceneGraph/NodeFactory.h>
#include <QKeyEvent>
#include <Core/Utilities/LogSystem.h>
#include "NodeMenu.h"
namespace GCL { namespace GUI {

SceneGraphWidget::SceneGraphWidget(SceneGraph::SceneGraphNode *_scenegraph, QWidget *parent)
    :scenegraph_(_scenegraph), SceneGraphItemModel_(new SceneGraphItemModel(_scenegraph)),QTreeView(parent)
{
    setModel(SceneGraphItemModel_.get());
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    for(int i=0; i < SceneGraphItemModel_->columnCount(); i++)
    {
        this->setColumnWidth(i,70);
    }
    connect(scenegraph_,SIGNAL(onUpdate()),this,SLOT(onUpdate()));
    connect(SceneGraphItemModel_.get(),SIGNAL(modelAboutToBeReset()), this, SLOT(slotModelAboutToBeReset()));
    connect(SceneGraphItemModel_.get(),SIGNAL(modelReset()), this, SLOT(slotModelReset()));
    connect(this, SIGNAL(doubleClicked(const QModelIndex)), this, SLOT(slotDoubleClicked(const QModelIndex&)));
    doItemsLayout();
}


//// events

void SceneGraphWidget::contextMenuEvent(QContextMenuEvent* _event){
    QModelIndex selectedIndex = currentIndex();
    if(selectedIndex.isValid()) {
        NodeMenu menu(static_cast<Node*>(selectedIndex.internalPointer()), this);
        menu.exec(_event->globalPos());
    }
}


void SceneGraphWidget::keyReleaseEvent(QKeyEvent* _event)
{
    QModelIndex selectedIndex = currentIndex();
    Node* node = 0;
    if(selectedIndex.isValid())
    {
        node = static_cast<Node*>(selectedIndex.internalPointer());
    }

    if(_event->key() == Qt::Key_Delete)
    {
        //delete node
        SceneGraphItemModel_->removeRow(currentIndex().row(), currentIndex().parent());
    }
    else {//pass it on
        scenegraph_->keyUp(_event->key(),_event->modifiers());
        scenegraph_->nodeUpdated();

    }
}

void SceneGraphWidget::onUpdate()
{
    reset();
    expandAll();
}



void SceneGraphWidget::slotDoubleClicked(const QModelIndex& _index)
{

    QModelIndex selectedIndex = _index;
    if(selectedIndex.isValid())
    {
        Node* node = static_cast<Node*>(selectedIndex.internalPointer());
        scenegraph_->focusOnNode(node);
        scenegraph_->nodeUpdated();

    }

}


void SceneGraphWidget::slotModelAboutToBeReset(){
    selectedIndex_ = currentIndex();
}

void SceneGraphWidget::slotModelReset()
{
    expandAll();
    if(selectedIndex_.isValid() && SceneGraphItemModel_->hasIndex(selectedIndex_.row(),selectedIndex_.column(),selectedIndex_.parent()))
    {
        setCurrentIndex(SceneGraphItemModel_->index(selectedIndex_.row(),selectedIndex_.column(),selectedIndex_.parent()));
    }
}



}} // namespace

#include "SceneGraphItemModel.h"

#include <Core/SceneGraph/SceneGraph.h>
#include <Core/Utilities/LogSystem.h>
using namespace GCL::SceneGraph;
namespace GCL { namespace GUI {

SceneGraphItemModel::SceneGraphItemModel(SceneGraph::SceneGraphNode *_scenegraph)
    :scenegraph_(_scenegraph)
{
}

SceneGraphItemModel::~SceneGraphItemModel()
{

}

QVariant SceneGraphItemModel::headerData(int _section, Qt::Orientation _orientation, int _role) const
{
    if (_orientation == Qt::Horizontal && _role == Qt::DisplayRole) {
        if(_section == COL_NAME) {
            return tr("Name");
        }
        else if(_section == COL_TYPE) {
            return tr("Type");
        }
        else if(_section == COL_SELECTED) {
            return tr("Selected");
        }
        else if(_section == COL_VISIBLE) {
            return tr("Visiable");
        }
        else {
            return QVariant();
        }
    }

    return QVariant();
}
Qt::ItemFlags SceneGraphItemModel::flags(const QModelIndex& _index) const {
    if (!_index.isValid()) {
        return Qt::ItemIsEnabled;
    }
    else if(_index.column()==COL_NAME) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable ;
    }
    else if(_index.column()==COL_SELECTED || _index.column()==COL_VISIBLE) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
    }
    else {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
}


QVariant SceneGraphItemModel::data(const QModelIndex& _index, int _role) const
{
    QVariant data;
    if (_index.isValid()) {
        Node* node = static_cast<Node*>(_index.internalPointer());

        if (_role == Qt::DisplayRole) {
            if(_index.column()==COL_NAME) {
                data = QVariant(node->objectName());
            }
            else if(_index.column()==COL_TYPE) {
                data = QVariant(node->getNodeTypeName());
            }
        }
        else if (_role == Qt::CheckStateRole)
        {
            if(_index.column()==COL_SELECTED)
            {
                if(node->selected())
                {
                    data = QVariant(Qt::Checked);
                }
                else
                {
                    data = QVariant(Qt::Unchecked);
                }
            }
            else if(_index.column()==COL_VISIBLE)
            {
                if(node->visible())
                {
                    data = QVariant(Qt::Checked);
                }
                else
                {
                    data = QVariant(Qt::Unchecked);
                }
            }
        }
    }

    return data;
}


QModelIndex SceneGraphItemModel::index(int _row, int _column, const QModelIndex& _parent) const
{

    //get node
    if(_parent.isValid()) {
        Node* parentNode = static_cast<Node*>(_parent.internalPointer());
        if(_row < parentNode->getNumberOfChildren() && _row >= 0)
        {
            Node * childNode = parentNode->getChild(_row);
            return createIndex(_row, _column, childNode);
        }
        else
        {
            return QModelIndex();
        }
    }
    else
    {
        //return root
        if(_row == 0)
        {
            return createIndex(_row, _column, scenegraph_);
        }
        else
        {
            return QModelIndex();
        }
    }

}


QModelIndex SceneGraphItemModel::parent(const QModelIndex& _index) const
{

    if(_index.isValid()) {
        Node* node = static_cast<Node*>(_index.internalPointer());
        if(node->hasParent())
        {
            //modelIndex
            int row = 0;
            Node* parent = node->getParent();
            if(parent->hasParent())
            {
                row = parent->getParent()->getChildIndex(parent);
            }
            //else: parent = root -> always in row 0
            return createIndex(row, 0, node->getParent());
        }
        else
        {
            return QModelIndex();
        }
    }
    else
    {
        return QModelIndex();
    }

}


int SceneGraphItemModel::rowCount(const QModelIndex& _parent) const {
    if (_parent.isValid())
    {
        return static_cast<Node*>(_parent.internalPointer())->getNumberOfChildren();
    }
    else
    {//toplevel parent can only have root node as row
        return 1;
    }
}


int SceneGraphItemModel::columnCount(const QModelIndex& _parent) const {
    //all rows have 4 columns
    Q_UNUSED(_parent)
    return 4;
}

bool SceneGraphItemModel::hasChildren(const QModelIndex &parent) const
{
    if(parent.isValid())
    {
        Node* node = static_cast<Node*>(parent.internalPointer());
        if(node->getNumberOfChildren())
            return true;
        else return false;
    }
    return true;
}




//// layout changing functions inherited from QAbstractItemModel

bool SceneGraphItemModel::removeRow(int _row, const QModelIndex& _parent){
    if(_parent.isValid())
    {
        Node* parentNode = static_cast<Node*>(_parent.internalPointer());

        beginResetModel();
        parentNode->getChild(_row)->deleteLater();
        parentNode->getChild(_row)->setParent(NULL);
        endResetModel();
        parentNode->nodeUpdated();
        return true;
    }
    else
    {//root (=scenegraph) node
        return false;
    }
}





//// editable model access functions inherited from QAbstractItemModel


bool SceneGraphItemModel::setData (const QModelIndex& _index, const QVariant& _value, int _role) {

    bool success = false;

    if (_index.isValid()) {
        Node* node = static_cast<Node*>(_index.internalPointer());

        if (_role == Qt::EditRole) {
            if(_index.column()==COL_NAME) {
                node->setObjectName(_value.toString());
                success = true;
            }
        }
        else if (_role == Qt::CheckStateRole)
        {
            if(_index.column()==COL_SELECTED)
            {
                if(_value.toInt() == Qt::Checked)
                {
                    node->setSelected(true);
                }
                else
                {
                    node->setSelected(false);
                }
                node->nodeUpdated();
                success = true;
            }
            else if(_index.column()==COL_VISIBLE)
            {
                if(_value.toInt() == Qt::Checked) {
                    node->setVisible(true);
                }
                else {
                    node->setVisible(false);
                }
                DebugLog<<node->objectName().toStdString()<<std::endl;

                node->nodeUpdated();
                success = true;
                DebugLog<<node->objectName().toStdString()<<std::endl;

            }
        }
    }

    //note: an update is emitted automatically due to the onNodeUpdate() routine

    return success;

}

}}

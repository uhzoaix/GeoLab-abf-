#include "PluginsItemModel.h"

#include <PluginCore/PluginManager/PluginManager.h>
#include <PluginCore/PluginInterface/PluginInterface.h>

#include <QtGui/QColor>


namespace GCL { namespace GUI {


PluginsItemModel::PluginsItemModel(PluginManager * const _pluginManager)
    : pluginManager_(_pluginManager) {

}

PluginsItemModel::~PluginsItemModel() {
}


//// Observer

//  void PluginsItemModel::onPluginManagerUpdate(PluginManager* _manager){
//    //TODO: if plugins are removed, this will not be enough!!
//    //in the scenegraph i solved this calling
//    //reset();
//    //however, this collapses all the tree nodes!!!
//    //find a way to handle this.
//    //as long as i cannot remove any plugins, this works fine.
//    Q_UNUSED(_manager)
//    emit layoutChanged();
//  }



//// read only model access functions inherited from QAbstractItemModel

QVariant PluginsItemModel::headerData(int _section, Qt::Orientation _orientation, int _role) const {
    Q_UNUSED(_section)
    Q_UNUSED(_orientation)
    Q_UNUSED(_role)
    return QVariant();
}


Qt::ItemFlags PluginsItemModel::flags(const QModelIndex& _index) const {
    Q_UNUSED(_index)
    return Qt::ItemIsEnabled;
}


QVariant PluginsItemModel::data(const QModelIndex& _index, int _role) const {
    QVariant data;
    if (_index.isValid()) {
        PluginInterface* plugin = static_cast<PluginInterface*>(_index.internalPointer());

        if (_role == Qt::DisplayRole) {
            if(plugin == 0) {//category
                data = QVariant(pluginManager_->getLoadedCategory(_index.row()));
            }
            else {//plugin
                //            DebugLog<<plugin->objectName().toStdString()<<std::endl;
                data = QVariant(plugin->getName());
            }
        }
        else if (_role == Qt::BackgroundRole) {
            if(plugin != 0) {
                if(plugin == pluginManager_->getActivePlugin()) {
                    data = QVariant(QColor(253,177,0));
                }
            }
        }
        else if (_role == Qt::ForegroundRole) {
            if(plugin != 0) {
                if(plugin == pluginManager_->getActivePlugin()) {
                    data = QVariant(QColor(Qt::white));
                }
            }
        }
    }

    return data;
}


QModelIndex PluginsItemModel::index(int _row, int _column, const QModelIndex& _parent) const {
    //get node
    if(_parent.isValid()) {//plugin
        if(_row < pluginManager_->getNumberOfLoadedPluginsInCategory(_parent.row())) {
            PluginInterface* plugin = pluginManager_->getLoadedPlugin(_parent.row(),_row);
            return createIndex(_row, _column, plugin);
        }
        else {
            return QModelIndex();
        }
    }
    else {//category
        if(_row >= 0 && _row < pluginManager_->getNumberOfLoadedCategories()) {
            return createIndex(_row, _column, (void*)0);
        }
        else {
            return QModelIndex();
        }
    }
}


QModelIndex PluginsItemModel::parent(const QModelIndex& _index) const {
    if(_index.isValid()) {
        PluginInterface* plugin = static_cast<PluginInterface*>(_index.internalPointer());
        if(plugin != 0) {//plugin
            //modelIndex
            int row = pluginManager_->getLoadedCategoryIndex(plugin->getCategory());
            return createIndex(row, 0, (void*)0);
        }
        else {//category
            return QModelIndex();
        }
    }
    else {
        return QModelIndex();
    }
}


int PluginsItemModel::rowCount(const QModelIndex& _parent) const {
    if (_parent.isValid()) {
        PluginInterface* parent = static_cast<PluginInterface*>(_parent.internalPointer());
        if(parent == 0) {//category
            return pluginManager_->getNumberOfLoadedPluginsInCategory(_parent.row());
        }
        else {//plugin
            return 0;
        }
    }
    else {//toplevel parent has number of categories as rows
        //        DebugLog<<pluginManager_->getNumberOfLoadedCategories()<<std::endl;

        return pluginManager_->getNumberOfLoadedCategories();
    }
}


int PluginsItemModel::columnCount(const QModelIndex& _parent) const {
    //all rows have 1 column
    Q_UNUSED(_parent)
    return 1;
}


}}//namespaces

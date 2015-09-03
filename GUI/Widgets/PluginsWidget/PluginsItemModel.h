#ifndef PLUGINS_ITEM_MODEL_H
#define PLUGINS_ITEM_MODEL_H

#include <QtCore/QAbstractItemModel>

#include <vector>


/*
* QAbstractItemModel implementation of the plugin manager
* It models
* - a hierarchical representation of the registered and visible plugins
* - the "row" of a ModelIndex is the categories index for the first stage,
*   and the plugins's index in its category for the second stage
* - there is only one column
* - the hierarchy has only two levels: the categories(internal pointer = 0)
*   and the plugins (internal pointer = plugin)
*/


namespace GCL {
  namespace Plugins {
    class PluginManager;
  }
}

using namespace GCL::Plugins;


namespace GCL { namespace GUI {


  class PluginsItemModel : public QAbstractItemModel {


  public:
    explicit PluginsItemModel(PluginManager * const _pluginManager);
    virtual ~PluginsItemModel();




    //// read only model access functions inherited from QAbstractItemModel
    virtual QVariant headerData(int _section, Qt::Orientation _orientation, int _role) const;
    virtual Qt::ItemFlags flags(const QModelIndex& _index) const;
    virtual QVariant data(const QModelIndex& _index, int _role) const;
    virtual QModelIndex index(int _row, int _column, const QModelIndex& _parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex& _index) const;
    virtual int rowCount(const QModelIndex& _parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex& _parent = QModelIndex()) const;


  private:
    PluginManager* pluginManager_;

  };

}}//namespaces

#endif

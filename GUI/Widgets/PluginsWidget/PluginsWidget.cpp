#include "PluginsWidget.h"

#include "PluginsItemModel.h"

#include <Core/Utilities/Settings.h>
#include <PluginCore/PluginManager/PluginManager.h>
#include <PluginCore/PluginInterface/PluginInterface.h>


#include <algorithm>


namespace GCL { namespace GUI {


  PluginsWidget::PluginsWidget(PluginManager *  _pluginManager, QWidget* parent )
               : QTreeView(parent),pluginsItemModel_(new PluginsItemModel(_pluginManager)),
                 pluginManager_(_pluginManager) {

    setModel(pluginsItemModel_.get());
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::NoSelection);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    connect(this, SIGNAL(doubleClicked(const QModelIndex)), this, SLOT(slotDoubleClicked(const QModelIndex&)));
//    connect(this, SIGNAL(clicked(const QModelIndex)), this, SLOT(slotClicked(const QModelIndex&)));
  }

  PluginsWidget::~PluginsWidget() {
  }


  //// settings

  void PluginsWidget::readSettings(Settings* const _settings){
    //NOTE: size automatically stored by main window

    //load expansion info
    QStringList expandedCategories_;
    _settings->read("PluginsWidget/expandedCategories", expandedCategories_);
    if(int numberOfCategories = pluginsItemModel_->rowCount()) {//toplevelrowcount
      for(int i=0; i<numberOfCategories; ++i) {
        const QModelIndex categoryIndex = pluginsItemModel_->index(i, 0);
        QString category = pluginsItemModel_->data(categoryIndex,Qt::DisplayRole).toString();
        if(std::find(expandedCategories_.begin(), expandedCategories_.end(), category) != expandedCategories_.end()) {
          expand(categoryIndex);
        }
      }
    }
  }

  void PluginsWidget::writeSettings(Settings* const _settings){
    //NOTE: size automatically stored by main window

    //save expansion info
    QStringList expandedCategories_;
    int numberOfCategories = pluginsItemModel_->rowCount();//toplevelrowcount
    for(int i=0; i<numberOfCategories; ++i) {
      const QModelIndex categoryIndex = pluginsItemModel_->index(i, 0);
      if(isExpanded(categoryIndex)) {
        QString category = pluginsItemModel_->data(categoryIndex,Qt::DisplayRole).toString();
        expandedCategories_.push_back(category);
      }
    }
    _settings->write("PluginsWidget/expandedCategories", expandedCategories_);
  }
 





  void PluginsWidget::slotDoubleClicked(const QModelIndex& _index) {
    //see whether plugin was selected
    QModelIndex selectedIndex = _index;
    if(selectedIndex.isValid()) {
      PluginInterface* plugin = static_cast<PluginInterface*>(selectedIndex.internalPointer());

      pluginManager_->setActivePlugin(plugin);

    }
  }




}}//namespaces

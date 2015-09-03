#ifndef PLUGINS_WIDGET_H
#define PLUGINS_WIDGET_H

/*
* Widget that displays the loaded plugins tree-sorted
* acording to their categories and invokes the pluginmanager on
* click/doubleclick/contextmenu(TODO).
* The invisible category is not shown.
*
*/

#include <QTreeView>

#include <memory>

class QWidget;

namespace GCL {
  class Settings;
  namespace Plugins {
    class PluginManager;
  }
}

using namespace GCL::Plugins;


namespace GCL { namespace GUI {

  class PluginsItemModel;
  class MainWindow;


  class PluginsWidget : public QTreeView {
    Q_OBJECT

  public:
    explicit PluginsWidget(PluginManager *  _pluginManager, QWidget* parent = 0);
    ~PluginsWidget();

    //// settings
    void readSettings(Settings* const _settings);
    void writeSettings(Settings * const _settings);


  private slots:
    void slotDoubleClicked(const QModelIndex& _index);
//    void slotClicked(const QModelIndex& _index);

  private:
    std::unique_ptr<PluginsItemModel> pluginsItemModel_;
    PluginManager* pluginManager_;

  };



}}//namespaces

#endif

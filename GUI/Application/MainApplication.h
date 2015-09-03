#ifndef GCL_MAINAPPLIACTION_H
#define GCL_MAINAPPLIACTION_H
#include <QObject>
#include <QList>
#include <QString>
#include <QStringList>
#include <memory>
#include "Core/gcl_global.h"
class QWidget;
class QActionGroup;
class QTranslator;
namespace GCL {
class Settings;
class ProgressTool;
namespace SceneGraph {
class SceneGraphNode;
}
namespace Plugins {
class PluginManager;
}
namespace GUI {
/*
* This the GCL application's heart and main class.
* It creates the mainwindow and all the widgets and glues them together.
* It also creates the scenegraph, the renderer and the pluginmanager.
* It provited methods to save and load settings over different application sessions.
* It is the main and only interface the plugininterface class uses
* to get to all the data of the plugins need.
*/
class MainWindow;
class HelpWidget;
class GCL_GUI_SHARED_EXPORT MainApplication : public QObject
{
    Q_OBJECT
public:
    MainApplication();
    ~MainApplication();
    SceneGraph::SceneGraphNode* getSceneGraph();
    const SceneGraph::SceneGraphNode* getSceneGraph() const;

    void addRecentFiles(const QString& filename);
    bool getRecentFiles(QStringList& recentfiles);
    void setRecentFiles(const QStringList& recentfiles);

    void addLibraryPath(const QString& path);
    QString applicationDirPath() const;

    ProgressTool* getProgressTool();

    Plugins::PluginManager *getPluginManager() const;

    Settings* getSettings() {return settings_.get();}

    void setMainWindow(QWidget* _mainwindow);
    QWidget* getMainWindow();

    void showHelpWidget();
signals:
    void signalUpdateRecentFiles();
private:
    std::shared_ptr<SceneGraph::SceneGraphNode> scenegraph_;
    std::shared_ptr<Settings> settings_;
    std::shared_ptr<HelpWidget> helpwidget_;
    std::shared_ptr<QWidget> mainwindow_;
    std::shared_ptr<QTranslator> translator_;
    ProgressTool* progressTool_;

    Plugins::PluginManager* pluginmanager_;
    QString applicationname_;
    QString applicationversion_;

};
}}
#endif // MAINAPPLIACTION_H

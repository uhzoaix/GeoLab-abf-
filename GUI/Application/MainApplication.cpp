#include "MainApplication.h"
#include "GuiNodeFactory.h"
#include <QApplication>
#include <Core/SceneGraph/SceneGraph.h>
#include <Core/Utilities/Settings.h>
#include <Core/Utilities/ProgressTool.h>
#include <PluginCore/PluginManager/PluginManager.h>
#include "GUI/Widgets/HelpWidget/HelpWidget.h"
#include <QWidget>
#include <QTranslator>
#include <Core/SceneGraph/RenderModes.h>
namespace GCL { namespace GUI {

MainApplication::MainApplication()
    : scenegraph_(new SceneGraph::SceneGraphNode()),settings_(new Settings("GCL_Mesh","0.1")), progressTool_(new ProgressTool(this)),
      pluginmanager_(new Plugins::PluginManager(qApp,scenegraph_.get(),this)),helpwidget_(new HelpWidget()),translator_(new QTranslator())
{
    pluginmanager_->initialTranslator(translator_.get());

    pluginmanager_->loadPlugins();

    pluginmanager_->setMainAppName("gcl_gui");
    pluginmanager_->setLanuguage("chs");
    pluginmanager_->setTranslatorForMainApp(pluginmanager_->language());
    helpwidget_->hide();
    scenegraph_->getRenderModes()->set_render_mode(SceneGraph::RenderModes::SMOOTH_SHADING);
    scenegraph_->setNodeFactory(std::shared_ptr<SceneGraph::NodeFactory>(new GuiNodeFactory(*this)));
}

MainApplication::~MainApplication()
{
    delete pluginmanager_;
    pluginmanager_ = NULL;
    scenegraph_->deleteLater();
    scenegraph_.reset();
}

SceneGraph::SceneGraphNode *MainApplication::getSceneGraph()
{
    return scenegraph_.get();
}

void MainApplication::addRecentFiles(const QString &filename)
{
    QStringList old_recentfiles;
    this->getRecentFiles(old_recentfiles);
    QStringList new_recentfiles;
    new_recentfiles.push_back(filename);
    for(int i=0; i < old_recentfiles.size() && i < 20; i++)
    {
        if(old_recentfiles[i] != filename)
        {
             new_recentfiles.push_back(old_recentfiles[i]);
        }
    }
    this->setRecentFiles(new_recentfiles);
    emit this->signalUpdateRecentFiles();
}

bool MainApplication::getRecentFiles(QStringList &recentfiles)
{
    return settings_->read("gcl/recentfiles",recentfiles);
}

void MainApplication::setRecentFiles(const QStringList &recentfiles)
{
    settings_->write("gcl/recentfiles",recentfiles);
}

void MainApplication::addLibraryPath(const QString &path)
{
    qApp->addLibraryPath(path);
}

QString MainApplication::applicationDirPath() const
{
    return qApp->applicationDirPath();
}

ProgressTool *MainApplication::getProgressTool()
{
    return progressTool_;
}
Plugins::PluginManager *MainApplication::getPluginManager() const
{
    return pluginmanager_;
}

void MainApplication::setMainWindow(QWidget *_mainwindow)
{
    mainwindow_ = std::shared_ptr<QWidget>(_mainwindow);
}

QWidget *MainApplication::getMainWindow()
{
    return mainwindow_.get();
}

void MainApplication::showHelpWidget()
{
    helpwidget_->showHelpWidget(pluginmanager_->getActiveHelpString(),pluginmanager_->getActiveAboutString());
}



}}

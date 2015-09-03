#include "MainWindow.h"
#include <QDockWidget>
#include <GUI/Application/MainApplication.h>
#include <GUI/Widgets/SceneGraphWidget/SceneGraphWidget.h>
#include <GUI/Widgets/RenderWidget/RenderWidget.h>
#include <Core/Utilities/LogSystem.h>
#include <Core/SceneGraph/SceneGraph.h>
#include <Core/SceneGraph/NodeFactory.h>
#include <Core/SceneGraph/OpenMeshNode.h>
#include <Core/SceneGraph/DepthImageNode.h>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QUrl>
#include <QMimeData>
#include <QStatusBar>
#include <Core/Utilities/ProgressTool.h>
#include <QProgressBar>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QActionGroup>
#include <QFileInfo>
#include <QToolBar>
#include <QFileDialog>
#include "PluginCore/PluginManager/PluginManager.h"
#include "Core/Utilities/ProgressTool.h"
#include <GUI/Widgets/PluginsWidget/PluginsWidget.h>
#include <QTabWidget>
#include <QSplitter>
#include "PluginCore/PluginInterface/MeshFilterInterface.h"
#include "GUI/Widgets/FilterDialog/FilterDialog.h"
#include <Core/DataStructures/OMesh.h>
#include <Core/SceneGraph/SceneGraph.h>
#include <Core/SceneGraph/OpenMeshNode.h>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/IO/IOInstances.hh>
#include <QApplication>
#include "GUI/Widgets/ShaderDialog/ShaderDialog.h"
namespace GCL {
using namespace Plugins;
namespace GUI {
MainWindow::MainWindow(MainApplication &_mainappliction) :
    mainapplication_(_mainappliction), QMainWindow(0),recentfile_menu_(NULL),recentfile_actions_(NULL)
{

    mainapplication_.setMainWindow(this);

    this->setAcceptDrops(true);
    this->createMainWidgets();
    this->createMenuAndToolBar();
    this->createStatusBar();

    connect(mainapplication_.getPluginManager(),SIGNAL(signalShowFilterDialog(MeshFilterInterface*)),this,SLOT(slotOpenFilterDialog(MeshFilterInterface*)));
    connect(mainapplication_.getPluginManager(),SIGNAL(signalActivePluginWidget(QWidget*)),this,SLOT(slotSetPluginWidget(QWidget*)));
    connect(&mainapplication_,SIGNAL(signalUpdateRecentFiles()),this,SLOT(slotUpdateRecentFiles()));
    connect(mainapplication_.getProgressTool(),SIGNAL(signalInitProgress(int,std::string)),this,SLOT(slotInitProgress(int,std::string)));
    connect(mainapplication_.getProgressTool(),SIGNAL(signalSetProgressPos(int)),this,SLOT(slotSetProgressPos(int)));
    connect(mainapplication_.getProgressTool(),SIGNAL(signalFinishProgress()),this,SLOT(slotFinishProgress()));

}

MainWindow::~MainWindow()
{
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("text/uri-list"))
        event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty())
        return;

    foreach(const QUrl& url, urls)
    {
        QString fileName = url.toLocalFile();
        if (fileName.isEmpty())
            continue;
        openFile(fileName);
    }

}


void MainWindow::resizeEvent(QResizeEvent *)
{

    QList<int> sizes;
    sizes.push_back(280);
    sizes.push_back(this->width() - sizes[0]);
    splitter_->setSizes(sizes);
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_F1)
    {
        mainapplication_.showHelpWidget();
        return;
    }
    mainapplication_.getSceneGraph()->keyUp(e->key(),e->modifiers());

}


void MainWindow::slotInitProgress(int maxSize, const std::string &progressName)
{
    mainprogressbar_->setRange(0,maxSize);
    mainprogressbar_->setValue(0);
    this->statusBar()->showMessage(progressName.c_str());

}

void MainWindow::slotSetProgressPos(int pos)
{
    mainprogressbar_->setValue(pos);
}

void MainWindow::slotFinishProgress()
{
    mainprogressbar_->setValue(0);
    this->statusBar()->clearMessage();
}

void MainWindow::slotOpenFileDialog()
{

    QString dirpath = "";
    QStringList recentFiles;
    mainapplication_.getRecentFiles(recentFiles);
    if(recentFiles.size() > 0)
    {
        QFileInfo fileinfo(recentFiles[0]);
        dirpath  = fileinfo.absoluteDir().absolutePath();
    }
    QStringList filenames = QFileDialog::getOpenFileNames(this, tr("Open Model file..."),dirpath,mainapplication_.getPluginManager()->getInputFormatFilters().join(";;"));
    foreach (const QString& filename, filenames) {
        openFile(filename);
    }
}

void MainWindow::slotSaveFileDialog()
{
    QString dirpath = "";
    QStringList recentFiles;
    mainapplication_.getRecentFiles(recentFiles);
    if(recentFiles.size() > 0)
    {
        QFileInfo fileinfo(recentFiles[0]);
        dirpath  = fileinfo.absoluteDir().absolutePath();
    }
    QString filename = QFileDialog::getSaveFileName(this, tr("Open Model file..."),dirpath,mainapplication_.getPluginManager()->getInputFormatFilters().join(";;"));

    SceneGraph::OpenMeshNode* openmeshnode = mainapplication_.getSceneGraph()->getFirstDescendant<SceneGraph::OpenMeshNode* >();
    if(!openmeshnode) return;
    OMesh& mesh = *openmeshnode->getMesh().get();
    DebugLog<< OpenMesh::IO::write_mesh(mesh,filename.toStdString())<<" "<<filename.toStdString()<<std::endl;

}

void MainWindow::slotOpenRecentFile(QAction *pAction)
{
    openFile(pAction->text());
}

void MainWindow::slotSetPluginWidget(QWidget *widget)
{
    while(tabwidget_->count() >= 3)
    {
        //        tabwidget_->widget(2)->deleteLater();
        tabwidget_->removeTab(2);
    }
    if(widget)
    {
        tabwidget_->addTab(widget,widget->windowTitle());
    }
}

void MainWindow::slotUpdateRecentFiles()
{
    if(recentfile_actions_)
    {
        recentfile_actions_->deleteLater();
        recentfile_actions_ = NULL;
    }
    recentfile_actions_ = new QActionGroup(recentfile_menu_);
    connect(recentfile_actions_,SIGNAL(triggered(QAction*)),this,SLOT(slotOpenRecentFile(QAction*)));

    QStringList recentFiles;
    mainapplication_.getRecentFiles(recentFiles);
    foreach (const QString& filename, recentFiles)
    {
        QAction* pAction  = new QAction(filename,recentfile_actions_);
        recentfile_actions_->addAction(pAction);
    }
    recentfile_menu_->addActions(recentfile_actions_->actions());
}

void MainWindow::slotOpenFilterDialog(MeshFilterInterface *filter)
{
    filter_dialog_->showFilterDialog(filter);
}

void MainWindow::openFile(const QString &filename)
{
    QFileInfo fileInfo(filename);
    if(!fileInfo.isFile()) return;
    int type = mainapplication_.getPluginManager()->getIOTypeByFilename(filename);
    if(type == Plugins::PluginManager::IO_MESH)
    {
        SceneGraph::Node* node = mainapplication_.getSceneGraph()->getNodeFactory()->createOpenMeshNode(filename,mainapplication_.getSceneGraph());
        if(node)
            node->setName(fileInfo.baseName());
    }
    else if(type == Plugins::PluginManager::IO_IMAGE)
    {
        SceneGraph::Node* node = mainapplication_.getSceneGraph()->getNodeFactory()->createDepthImageNode(filename,mainapplication_.getSceneGraph());
        if(node)
            node->setName(fileInfo.baseName());
    }
    else
    {
        return;
    }
    mainapplication_.getSceneGraph()->nodeUpdated();
}


void MainWindow::createMainWidgets()
{
    //    mainDockWidget_ = new QDockWidget(tr("Widgets"),this);
    splitter_ = new QSplitter(Qt::Horizontal,this);
    tabwidget_ = new QTabWidget(this);

    QWidget* renderwidget = new RenderWidget(mainapplication_.getSceneGraph(),this);
    renderwidget->setFocus();
    splitter_->addWidget(tabwidget_);
    //    mainDockWidget_->setWidget(tabwidget_);
    tabwidget_->addTab(new SceneGraphWidget(mainapplication_.getSceneGraph(),tabwidget_),
                       tr("SceneGraph"));
    tabwidget_->addTab(new PluginsWidget(mainapplication_.getPluginManager(),tabwidget_),tr("PluginManager"));
    splitter_->addWidget(renderwidget);
    this->setCentralWidget(splitter_);

}

void MainWindow::createMenuAndToolBar()
{
    // Tool Bar
    toolbar_ = this->addToolBar(tr("Main ToolBar"));
    // File Menu
    QMenu* pFileMenu = new QMenu(tr("File(&F)"),this);
    this->menuBar()->addMenu(pFileMenu);
    this->createFileActions(pFileMenu);

    // Edit Menu
    QMenu* pEditMenu = new QMenu(tr("Edit(&E)"),this);
    this->menuBar()->addMenu(pEditMenu);


    // Filter Menu
    QMenu* pFilterMenu = new QMenu(tr("Filter(&O)"),this);
    this->menuBar()->addMenu(pFilterMenu);
    this->createFilterActions(pFilterMenu);

    // Advanced Plugin Menu
    QMenu* pAdvancedMenu = new QMenu(tr("Plugins(&P)"),this);
    this->menuBar()->addMenu(pAdvancedMenu);

    //  View Menu
    QMenu* pViewMenu = new QMenu(tr("View(&V)"),this);
    this->menuBar()->addMenu(pViewMenu);
    this->createViewActions(pViewMenu);


    // Tool Menu
    QMenu* pToolMenu = new QMenu(tr("Tool(&T)"),this);
    this->menuBar()->addMenu(pToolMenu);


    //  Widget Menu
    QMenu* pWidgetMenu = new QMenu(tr("Widget(&W)"),this);
    this->menuBar()->addMenu(pWidgetMenu);

    // Help Menu
    QMenu* pHelpMenu = new QMenu(tr("Help(&H)"),this);
    this->menuBar()->addMenu(pHelpMenu);

}

void MainWindow::createStatusBar()
{
    mainprogressbar_ = new QProgressBar(this);
    this->statusBar()->addPermanentWidget(mainprogressbar_);
}

void MainWindow::createFileActions(QMenu *pFileMenu)
{
    // Open
    QAction* pAction = new QAction(tr("Open Mesh"),this);
    pAction->setShortcut(QKeySequence::Open);
    pAction->setIcon(QIcon(":/fileopen"));
    pAction->setStatusTip(tr("Open Mesh Or Image"));
    connect(pAction,SIGNAL(triggered()),this,SLOT(slotOpenFileDialog()));
    pFileMenu->addAction(pAction);
    toolbar_->addAction(pAction);
    // Save
    pAction = new QAction(tr("Save"),this);
    pAction->setIcon(QIcon(":/filesave"));
    pAction->setShortcut(QKeySequence::Save);
    pAction->setStatusTip(tr("Save Mesh"));
    connect(pAction,SIGNAL(triggered()),this,SLOT(slotSaveFileDialog()));
    pFileMenu->addAction(pAction);
    toolbar_->addAction(pAction);

    // RecentFile
    recentfile_menu_ = new QMenu(tr("Recent Files"),pFileMenu);
    pFileMenu->addMenu(recentfile_menu_);
    this->slotUpdateRecentFiles();

}

void MainWindow::createFilterActions(QMenu *pFilterMenu)
{
    QList<Plugins::MeshFilterInterface* > filters = mainapplication_.getPluginManager()->getFilterList();
    filter_dialog_ = new FilterDialog(this);
    filter_dialog_->hide();
    connect(filter_dialog_,SIGNAL(readyToApplyFilter(Plugins::MeshFilterInterface*)),mainapplication_.getPluginManager(),SLOT(slotApplyFilter(Plugins::MeshFilterInterface*)));
    QMap<QString, QMenu* > filter_menus;
    for(size_t i=0; i < Plugins::MeshFilterInterface::getNumberOfFilterCategories(); i++)
    {
        QString category = Plugins::MeshFilterInterface::getFilterCategoryName(i);
        QMenu* menu = new QMenu(category,pFilterMenu);
        filter_menus.insert(category,menu);
        pFilterMenu->addMenu(menu);
    }

    foreach(Plugins::MeshFilterInterface* filter, filters)
    {
        QMenu* menu = filter_menus.value(filter->getCategory(),NULL);
        if(!menu)
        {
            menu = pFilterMenu;
        }

        menu->addAction(new OpenFilterDialogAction(filter,filter_dialog_,this));
    }
}

void MainWindow::createViewActions(QMenu *pViewMenu)
{
    QMenu* shaderMenu = new QMenu(tr("Shader"),pViewMenu);
    pViewMenu->addMenu(shaderMenu);
    QDir shadersDir = QDir(qApp->applicationDirPath());
    bool ret = shadersDir.cd("shaders");
    if(!ret)
    {
        WarnLog<<"Unable to find the shaders directory.\n No shaders will be loaded"<<std::endl;
    }
    ShaderDialog* pShaderDialog = new ShaderDialog(mainapplication_.getSceneGraph(), this);
    QActionGroup* pActionGroup = new QActionGroup(this);
    OpenShaderDialogAction* pAction = new OpenShaderDialogAction("None",pShaderDialog,this);
    pAction->setChecked(true);
    pActionGroup->addAction(pAction);
    foreach(const QString& filename, shadersDir.entryList(QDir::Files))
    {
        if(filename.endsWith(".gdp"))
        {
            pAction =  new OpenShaderDialogAction(shadersDir.absolutePath()
                                                  + "/" + filename,pShaderDialog,this);
            pActionGroup->addAction(pAction);
        }
    }
    shaderMenu->addActions(pActionGroup->actions());
}


}}

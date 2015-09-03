#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Core/gcl_global.h"
#include <QMainWindow>
class QDockWidget;
class QProgressBar;
class QMenu;
class QTabWidget;
class QActionGroup;
class QSplitter;
namespace GCL {
namespace Plugins {
class MeshFilterInterface;
}
using namespace Plugins;
namespace GUI {
/**
 * @brief The MainWindow class
 *      It's convinient class of a mainwindow. If you want to change the style of your own app,
 * you can just generated a new mainwindow refer to this class;
 * \author Shiwei
 *
 */
class MainApplication;
class FilterDialog;
class GCL_GUI_SHARED_EXPORT MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(MainApplication& _mainapplication);
    ~MainWindow();

    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

protected:
    void resizeEvent(QResizeEvent *);
    void keyPressEvent(QKeyEvent *e);
public slots:
    void slotInitProgress(int maxSize, const std::string& progressName);
    void slotSetProgressPos(int pos);
    void slotFinishProgress();

    void slotOpenFileDialog();
    void slotSaveFileDialog();
    void slotOpenRecentFile(QAction* pAction);
    void slotSetPluginWidget(QWidget* widget);

    void slotUpdateRecentFiles();
    void slotOpenFilterDialog(MeshFilterInterface* filter);
private:
    void openFile(const QString& filename);
    void createMainWidgets();
    void createMenuAndToolBar();
    void createStatusBar();

    void createFileActions(QMenu* pFileMenu);
    void createFilterActions(QMenu* pFilterMenu);
    void createViewActions(QMenu* pViewMenu);

    QSplitter* splitter_;
    QDockWidget* mainDockWidget_;
    MainApplication& mainapplication_;
    QProgressBar* mainprogressbar_;

    ///* tabwidget_: tab 0: SceneGraphWidget; 1: pluginManagerWidget; 2: None or ActivePluginWidget
    QTabWidget* tabwidget_;
    QMenu* recentfile_menu_;
    QActionGroup* recentfile_actions_;
    QToolBar* toolbar_;
    FilterDialog* filter_dialog_;
};
}}
#endif // MAINWINDOW_H

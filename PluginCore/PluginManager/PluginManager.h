#ifndef GCL_PLUGINMANAGER_H
#define GCL_PLUGINMANAGER_H
#include <QObject>
#include <QMap>
#include <QStringList>

#include <Core/gcl_global.h>
class QWidget;
class QApplication;
class QTranslator;
namespace GCL {
namespace SceneGraph {
class SceneGraphNode;
}

namespace GUI {
class MainApplication;
}
namespace Plugins {
/**
 * @brief The PluginManager class
 *      插件管理类:分别参考了 meshlab 和 Dolphin 的 PluginManager
 * @author Shiwei
 */
class PluginInterface;
class IOInterface;
class MeshIOInterface;
class ImageIOInterface;
class MeshFilterInterface;
class GCL_PLUGIN_SHARED_EXPORT PluginManager : public QObject
{
    Q_OBJECT
public:
    enum IO_TYPE
    {
        IO_NONE = 0,
        IO_MESH = 1,
        IO_IMAGE = 2
    };

    PluginManager(QApplication* _qApp, SceneGraph::SceneGraphNode* _scenegraph, QObject *parent = 0);
    ~PluginManager();
    void loadPlugins();
    QString getBaseDirPath();
    QString getDefaultPluginDirPath();
public:
    void setLanuguage(const QString& _language) {language_ = _language;}

    QString& language() {return language_;}

    const QString& language() const {return language_;}

    void setMainAppName(const QString& _mainappname) {mainappname_ = _mainappname;}

    void initialTranslator(QTranslator* _qtranslator) {qTranslator_ = _qtranslator;}
    /**
     * @brief setTranslatorForPlugin
     * @param _plugin_filename
     * @param _language  the abbreviate of name of language, e.g. en, chs, ru, de, fr, po ...
     */
    void setTranslatorForPlugin(const QString& _plugin_filename, const QString& _language = "chs");
    /**
     * @brief setTranslatorForMainApp
     * @param _language
     */
    void setTranslatorForMainApp(const QString& _language = "chs");
public:
    /// IO
    int getIOTypeByFilename(const QString& filename);
    MeshIOInterface* getImportMeshPluginByFilename(const QString& filename);
    MeshIOInterface* getExportMeshPluginByFilename(const QString& filename);
    ImageIOInterface* getImportImagePluginByFilename(const QString& filename);
    ImageIOInterface* getExportImagePluginByFilename(const QString &filename);

    inline const QStringList& getInputFormatFilters() {return FormatFilters;}
    inline const QStringList& getOutputFormatFilters() {return FormatFilters;}

    void setActivePlugin(PluginInterface* _plugin);
    inline PluginInterface* getActivePlugin() { return activePlugin_; } //0 if none active

    QString getActiveHelpString() const;
    QString getActiveAboutString() const;

    int getNumberOfLoadedCategories() const;    /// all are visible plugins
    const QString getLoadedCategory(int _categoryIndex) const;
    int getLoadedCategoryIndex(const QString& _category) const;//-1 if not found


    int getNumberOfLoadedPugins() const;
    int getNumberOfLoadedPluginsInCategory(int _index ) const;
    PluginInterface* getLoadedPlugin(int _index);//0 if category not found
    PluginInterface* getLoadedPlugin(int _categoryIndex, int _index);
    int getLoadedPluginIndex(PluginInterface* _plugin) const;//-1 if not found
    PluginInterface* getLoadedPluginByName(const QString& _name);//0 if not found
    QList<MeshFilterInterface* > getFilterList();

    void loadPlugin(PluginInterface* _plugin);
signals:
    void signalActivePluginWidget(QWidget* widget);
    void signalShowFilterDialog(MeshFilterInterface* filter);
public slots:
    void slotApplyFilter(Plugins::MeshFilterInterface* filter);
private:


    void knownIOMeshFormats();
    typedef QList<PluginInterface*> PluginVector;
    typedef QPair<QString,PluginVector> CategoryPluginPair;
    QList< CategoryPluginPair >  loadedCategories_;//starts with invisible plugins

    QMap<QString,IOInterface*> allKnowFormats;
    QStringList FormatFilters;

    PluginInterface* activePlugin_;


    SceneGraph::SceneGraphNode* scenegraph_;
    QApplication* qApp_;
    QTranslator* qTranslator_;

    QString language_;
    QString mainappname_;
};

}}
#endif // PLUGINMANAGER_H

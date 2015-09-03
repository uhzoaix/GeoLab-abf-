#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H
#include <QObject>
#include <QStringList>
#include <memory>
#include "Core/gcl_global.h"

class QWidget;
class QTranslator;
namespace GCL {

class OMesh;
namespace SceneGraph {
class SceneGraphNode;
}


namespace Plugins {
class GCL_PLUGIN_SHARED_EXPORT PluginInterface : public QObject
{
    Q_OBJECT
public:
    enum PluginType
    {
        PLUGIN_IO = 0, ///*  mesh io || image io
        PLUGIN_FILTER = 1, ///* apply filter to modify mesh only with a parameters dialog;
        PLUGIN_NORMAL = 2 ///* more complicated interaction, has a PluginWidget
    };

    PluginInterface(QObject* parent);
    virtual ~PluginInterface();
//    void init(GUI::MainApplication* _mainapplication);
    void setSceneGraph(SceneGraph::SceneGraphNode* _scenegraph) {scenegraph_ = _scenegraph;}
    SceneGraph::SceneGraphNode* getSceneGraph() {return scenegraph_;}
    virtual QWidget* getPluginWidget() {return NULL;}


public:
    virtual int getType() const = 0;
    virtual QString getCategory() const = 0;

    QString getName() const {return objectName();}



    /**
     * @brief getPluginFileName
     *      The plugin file name included the relative path of the plugin without suffix, e.g. "./plugins/io_base". It is used to find the plugins, their translator files,...
     * @return the filename of plugin
     */
    virtual QString getPluginFileName() const {return "";}


    virtual void setActive() {}
    virtual void unsetActive() {}

    virtual QString getHelpString() const {return "";}
    virtual QString getAboutString() const;



//    void initProgress(int maxSize, const std::string& progressName = "");
//    void setProgressPos(int pos);
//    void finishProgress();
private:
    SceneGraph::SceneGraphNode* scenegraph_;
};


class GCL_PLUGIN_SHARED_EXPORT IOInterface : public PluginInterface
{
    Q_OBJECT
public:
    class Format
    {
    public:
        Format(QString description,QString ex): description(description){extensions << ex;}
        QString description;
        QStringList extensions;
    };
    IOInterface(QObject* parent);
    virtual ~IOInterface();
    QString getCategory() const;

    int getType() const {return PLUGIN_IO;}
    virtual QList<Format> importFormats() const = 0;
    virtual QList<Format> exportFormats() const = 0;

    bool formatImportable(const QString& ex);
    bool formatExportable(const QString& ex);
};




}}
#if (QT_VERSION  >= 0x050000)
#define GCL_MESH_PLUGIN_IID_EXPORTER(x) Q_PLUGIN_METADATA(IID x)
#define GCL_MESH_PLUGIN_NAME_EXPORTER(x)
#else
#define GCL_MESH_PLUGIN_IID_EXPORTER(x)
#define GCL_MESH_PLUGIN_NAME_EXPORTER(x) Q_EXPORT_PLUGIN(x)
#endif



#define GCL_INTERFACE_IID  "gcl.mesh.plugininterface/1.0"

Q_DECLARE_INTERFACE(GCL::Plugins::PluginInterface,
                    GCL_INTERFACE_IID)
#endif // PLUGININTERFACE_H

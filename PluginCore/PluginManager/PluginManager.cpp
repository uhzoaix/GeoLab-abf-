#include "PluginManager.h"
#include <QPluginLoader>
#include <QDir>
#include "PluginCore/PluginInterface/MeshIOInterface.h"
#include "PluginCore/PluginInterface/MeshFilterInterface.h"
#include "PluginCore/PluginInterface/ImageIOInterface.h"
#include "PluginCore/PluginInterface/NormalPluginInterface.h"
#include <assert.h>
#include <Core/Utilities/LogSystem.h>
#include <QDebug>
#include <QApplication>
#include <Core/SceneGraph/SceneGraph.h>
#include <QWidget>
#include <QTranslator>
static QString DLLExtension() {
#if defined(Q_OS_WIN)
    return QString("dll");
#elif defined(Q_OS_MAC)
    return QString("dylib");
#else
    return QString("so");
#endif
    assert(0 && "Unknown Operative System. Please Define the appropriate dynamic library extension");
    return QString();
}

//static QString DLLFileNamePreamble() {
//#if defined(Q_OS_WIN)
//    return QString("");
//#elif defined(Q_OS_MAC)
//    return QString("lib");
//#else
//    return QString("lib");
//#endif
//    assert(0 && "Unknown Operative System. Please Define the appropriate dynamic library preamble");
//    return QString();
//}

namespace GCL { namespace Plugins {


PluginManager::PluginManager(QApplication *_qApp, SceneGraph::SceneGraphNode *_scenegraph, QObject *parent)
    :QObject(parent),activePlugin_(NULL),qApp_(_qApp),scenegraph_(_scenegraph),qTranslator_(NULL)
{
}

PluginManager::~PluginManager()
{
    DebugLog<<"Release PluginManager~!"<<std::endl;
}



void PluginManager::loadPlugins()
{
    QDir pluginsDir=QDir(getDefaultPluginDirPath());
    // without adding the correct library path in the mac the loading of jpg (done via qt plugins) fails
    qApp_->addLibraryPath(getDefaultPluginDirPath());
    qApp_->addLibraryPath(getBaseDirPath());
    QStringList pluginfilters;

    pluginfilters << QString("*." + DLLExtension());


    //only the file with extension pluginfilters will be listed by function entryList()
    pluginsDir.setNameFilters(pluginfilters);

    qDebug( "Currentd Plugins Dir is: %s ",qPrintable(pluginsDir.absolutePath()));

    foreach (QString fileName, pluginsDir.entryList(QDir::Files))
    {
        QString absfilepath = pluginsDir.absoluteFilePath(fileName);
        {
            QPluginLoader loader(absfilepath);
            QString plugin_filename ="plugins/" + QFileInfo(absfilepath).baseName();
            this->setTranslatorForPlugin(plugin_filename,language_);
            PluginInterface *plugin = dynamic_cast<PluginInterface *>(loader.instance());
            if (plugin)
            {
                loadPlugin(plugin);
                //                DebugLog <<"here:"<< plugin->objectName().toStdString()<<std::endl;
            }
            else
                DebugLog << loader.errorString().toStdString();
        }
    }
    foreach(PluginInterface* plugin, this->findChildren<PluginInterface* >())
    {
        plugin->setSceneGraph(scenegraph_);
    }
    knownIOMeshFormats();
}

QString PluginManager::getBaseDirPath()
{
    QDir baseDir(qApp->applicationDirPath());

#if defined(Q_OS_WIN)
    // Windows:
    // during development with visual studio binary could be in the debug/release subdir.
    // once deployed plugins dir is in the application directory, so
    if (baseDir.dirName() == "debug" || baseDir.dirName() == "release")		baseDir.cdUp();
#endif

#if defined(Q_OS_MAC)
    // Mac: during developmentwith xcode  and well deployed the binary is well buried.
    for(int i=0;i<6;++i){
        if(baseDir.exists("plugins")) break;
        baseDir.cdUp();
    }
    qDebug("The base dir is %s",qPrintable(baseDir.absolutePath()));
#endif
    return baseDir.absolutePath();
}

QString PluginManager::getDefaultPluginDirPath()
{
    QDir pluginsDir(getBaseDirPath());
    if(!pluginsDir.exists("plugins"))
        //QMessageBox::warning(0,"Meshlab Initialization","Serious error. Unable to find the plugins directory.");
        qDebug("GCL MeshFrameWork Initialization: Serious error. Unable to find the plugins directory.");
    pluginsDir.cd("plugins");
    return pluginsDir.absolutePath();
}

int PluginManager::getIOTypeByFilename(const QString &filename)
{
    if(getImportMeshPluginByFilename(filename))
    {
        return IO_MESH;
    }
    if(getImportImagePluginByFilename(filename))
    {
        return IO_IMAGE;
    }
    return IO_NONE;
}

MeshIOInterface *PluginManager::getImportMeshPluginByFilename(const QString &filename)
{
    QFileInfo fileinfo(filename);
    if(allKnowFormats.count(fileinfo.suffix()))
    {
        IOInterface* plugin = allKnowFormats.value(fileinfo.suffix());
        return qobject_cast<MeshIOInterface *>(plugin);

    }
    else
    {
        return NULL;
    }
}

MeshIOInterface *PluginManager::getExportMeshPluginByFilename(const QString &filename)
{
    QFileInfo fileinfo(filename);
    if(allKnowFormats.count(fileinfo.suffix()))
    {
        IOInterface* plugin = allKnowFormats.value(fileinfo.suffix());
        return qobject_cast<MeshIOInterface *>(plugin);
    }
    else
    {
        return NULL;
    }
}

ImageIOInterface *PluginManager::getImportImagePluginByFilename(const QString &filename)
{
    QFileInfo fileinfo(filename);
    if(allKnowFormats.count(fileinfo.suffix()))
    {
        IOInterface* plugin = allKnowFormats.value(fileinfo.suffix());
        return qobject_cast<ImageIOInterface *>(plugin);

    }
    else
    {
        return NULL;
    }
}

ImageIOInterface *PluginManager::getExportImagePluginByFilename(const QString &filename)
{
    QFileInfo fileinfo(filename);
    if(allKnowFormats.count(fileinfo.suffix()))
    {
        IOInterface* plugin = allKnowFormats.value(fileinfo.suffix());
        return qobject_cast<ImageIOInterface *>(plugin);

    }
    else
    {
        return NULL;
    }
}

void PluginManager::setActivePlugin(PluginInterface *_plugin)
{
    if(activePlugin_)
    {
        activePlugin_->unsetActive();
    }
    activePlugin_ = _plugin;
    if(activePlugin_)
    {
        setTranslatorForPlugin(activePlugin_->getPluginFileName(),language_);

        activePlugin_->setActive();
        MeshFilterInterface* filter = qobject_cast<MeshFilterInterface* >(_plugin);
        if(filter)
        {
            emit this->signalShowFilterDialog(filter);
        }
        //        else
        //        {
        emit this->signalActivePluginWidget(activePlugin_->getPluginWidget());

        //        }
        //        setTranslatorForMainApp();
    }
}

QString PluginManager::getActiveHelpString() const
{
    if(activePlugin_)
    {
        return activePlugin_->getHelpString();
    }

    QString text =QString("<center><h1>")+ tr("GCL Geometry Processing Software") + QString("</h1></center>");
    text += QString("<p>") + tr("Filters: Smoothing, Remeshing, Reconstruction, Repairing") +QString("</p>");
    text += QString("<p>") + tr("Plugins: Relief, EasyModeling, and others") + QString("</p>");


    return text;
}

QString PluginManager::getActiveAboutString() const
{
    if(activePlugin_)
    {
        return activePlugin_->getAboutString();
    }
    QString text = "<center>";
    text += "<h1>Graphics&Geometric Computing Laboratory @USTC</h1>";
    text += "<h2>Prof. Ligang Liu</h2>";
    text += "<h3>lgliu@ustc.edu.cn</h3>";
    text += "</center>";
    QString pstart("<p style='font-size:14px;'>");
    QString pend("</p>");
    QString blank("&nbsp;&nbsp;");
    text += pstart + blank + tr("GCLab is an extensible mesh processing system, aimed to help users process 3D triangular meses, help programmers enhance their software with plugins and help researchers (GCLers) make their ideas quickly. ") + pend;

    return text;
}

int PluginManager::getNumberOfLoadedCategories() const
{
    return loadedCategories_.size();
}

const QString PluginManager::getLoadedCategory(int _categoryIndex) const
{
    return loadedCategories_.at(_categoryIndex).first;

}

int PluginManager::getLoadedCategoryIndex(const QString &_category) const
{
    int index = 0;
    foreach (const CategoryPluginPair& pair, loadedCategories_) {
        if(pair.first == _category){
            return index;
        }
        index++;
    }
    return -1;
}

int PluginManager::getNumberOfLoadedPugins() const
{
    int index =0;
    foreach (const CategoryPluginPair& pair, loadedCategories_) {
        index += pair.second.size();
    }
    return index;
}

int PluginManager::getNumberOfLoadedPluginsInCategory(int _index) const
{
    return loadedCategories_.at(_index).second.size();
}

PluginInterface *PluginManager::getLoadedPlugin(int _index)
{
    if(_index < 0) return NULL;
    foreach ( CategoryPluginPair pair, loadedCategories_) {
        if(_index < pair.second.size())
        {
            return pair.second.at(_index);
        }
        _index -= pair.second.size();
    }
    return NULL;
}

PluginInterface *PluginManager::getLoadedPlugin(int _categoryIndex, int _index)
{
    return loadedCategories_[_categoryIndex].second[_index];
}

int PluginManager::getLoadedPluginIndex(PluginInterface *_plugin) const
{
    int index = 0;
    foreach (const CategoryPluginPair& pair, loadedCategories_) {
        for(int i=0; i < pair.second.size(); i++)
        {
            if(_plugin == pair.second.at(i))
            {
                return index + i;
            }
        }
        index += pair.second.size();
    }
    return index;
}

PluginInterface *PluginManager::getLoadedPluginByName(const QString &_name)
{
    foreach (CategoryPluginPair pair, loadedCategories_) {
        for(int i=0; i < pair.second.size(); i++)
        {
            if(_name == pair.second.at(i)->getName())
            {
                return pair.second.at(i);
            }
        }
    }
    return NULL;
}

QList<MeshFilterInterface *> PluginManager::getFilterList()
{
    return findChildren<MeshFilterInterface* >();
}

void PluginManager::loadPlugin(PluginInterface *_plugin)
{

    //    DebugLog<<"loadPlugin"<<std::endl;
    if(_plugin)
    {
        _plugin->setParent(this);
        IOInterface* io_interface  = qobject_cast<IOInterface* >(_plugin);
        if(io_interface) return;
        int index = (getLoadedCategoryIndex(_plugin->getCategory()));
        //        DebugLog<<_plugin->getCategory().toStdString()<<" "<<index<<std::endl;
        if(index == -1)
        {
            CategoryPluginPair newCategory;
            newCategory.first = _plugin->getCategory();
            index = loadedCategories_.size();
            loadedCategories_.push_back(newCategory);
        }
        loadedCategories_[index].second.push_back(_plugin);

    }


}

void PluginManager::slotApplyFilter(Plugins::MeshFilterInterface *filter)
{
    if(filter)
    {
        if(!filter->apply())
        {
            DebugLog<<"Failed to  apply filter: "<<filter->getName().toStdString()<<std::endl;
        }
    }
}

void PluginManager::setTranslatorForPlugin(const QString &_plugin_filename, const QString &_language)
{
    if(!qTranslator_) return;
    QString filename = getBaseDirPath() +"/" + _plugin_filename+".qm";
    qTranslator_->load(filename);
    qApp->installTranslator(qTranslator_);
}

void PluginManager::setTranslatorForMainApp(const QString &_language)
{
    if(!qTranslator_) return;
    qTranslator_->load(mainappname_+"_"+_language+".qm");
    qApp->installTranslator(qTranslator_);
}

void PluginManager::knownIOMeshFormats()
{
    QList<IOInterface* > IOPlug =  findChildren<IOInterface* >();
    QStringList* formatFilters = &FormatFilters;;
    QString allKnownFormatsFilter = QObject::tr("All known formats (");
    QMap<QString,IOInterface*>* map = &allKnowFormats;

    for(QList<IOInterface*>::iterator itIOPlugin = IOPlug.begin();itIOPlugin != IOPlug.end();++itIOPlugin)
    {
        IOInterface* pMeshIOPlugin = *itIOPlugin;
        QList<IOInterface::Format> format =pMeshIOPlugin->importFormats();


        for(QList<IOInterface::Format>::iterator itf = format.begin();itf != format.end();++itf)
        {
            IOInterface::Format currentFormat = *itf;

            QString currentFilterEntry = currentFormat.description + " (";

            //a particular file format could be associated with more than one file extension
            QStringListIterator itExtension(currentFormat.extensions);
            while (itExtension.hasNext())
            {
                QString currentExtension = itExtension.next().toLower();
                if (!map->contains(currentExtension))
                {
                    map->insert(currentExtension, pMeshIOPlugin);
                    allKnownFormatsFilter.append(QObject::tr(" *."));
                    allKnownFormatsFilter.append(currentExtension);
                }
                currentFilterEntry.append(QObject::tr(" *."));
                currentFilterEntry.append(currentExtension);
            }
            currentFilterEntry.append(')');
            formatFilters->append(currentFilterEntry);
        }

    }
    allKnownFormatsFilter.append(')');
    if(formatFilters!=NULL)
    {
        formatFilters->push_front(allKnownFormatsFilter);
    }


}

}}

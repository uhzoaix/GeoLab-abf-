#include "PluginInterface.h"
#include <Core/Utilities/ProgressTool.h>
#include <Core/Utilities/LogSystem.h>
#include <QTranslator>
namespace GCL { namespace Plugins {
PluginInterface::PluginInterface(QObject *parent)
    :scenegraph_(NULL),QObject(parent)
{
}

PluginInterface::~PluginInterface()
{
    //    DebugLog<<"Release Plugin: "<<this->objectName().toStdString()<<std::endl;
}

QString PluginInterface::getAboutString() const
{
    QString text = "<center>";
    text += "<h1>Graphics&Geometric Computing Laboratory @USTC</h1>";
    text += "<h2>Prof. Ligang Liu</h2>";
    text += "<p>lgliu@ustc.edu.cn</p>";
    text += "</center>";
    return text;
}


//void PluginInterface::initProgress(int maxSize, const std::string &progressName)
//{
//    if(mainapplication_)
//    {
//        mainapplication_->getProgressTool()->initProgress(maxSize,progressName);
//    }
//}

//void PluginInterface::setProgressPos(int pos)
//{
//    if(mainapplication_)
//    {
//        mainapplication_->getProgressTool()->setProgressPos(pos);
//    }
//}

//void PluginInterface::finishProgress()
//{
//    if(mainapplication_)
//    {
//        mainapplication_->getProgressTool()->finishProgress();
//    }
//}

IOInterface::IOInterface(QObject *parent)
    :PluginInterface(parent)
{

}

IOInterface::~IOInterface()
{

}

QString IOInterface::getCategory() const
{
    return "File";
}

bool IOInterface::formatImportable(const QString &ex)
{
    QString tmp = ex.toLower();
    foreach(const Format& format, importFormats())
    {
        foreach(const QString& ext, format.extensions)
        {
            if(tmp == ext) return true;
        }
    }
    return false;
}

bool IOInterface::formatExportable(const QString &ex)
{
    QString tmp = ex.toLower();
    foreach(const Format& format, exportFormats())
    {
        foreach(const QString& ext, format.extensions)
        {
            if(tmp == ext) return true;
        }
    }
    return false;
}

}}

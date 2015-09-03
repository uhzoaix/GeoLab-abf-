#include "Settings.h"
#include <QSettings>
#include <QFile>
#include <QTextStream>
#include <QDataStream>
#include <Core/Utilities/LogSystem.h>
namespace GCL {
Settings* Settings::settings_ = NULL;
Settings::Settings(const QString &_applicationName, const QString &_applicationVersion)
    : applicationName_((_applicationName)),
      applicationVersion_((_applicationVersion))
{
    settings_ = this;
    load();
}

Settings::~Settings(){
    save();
}


void Settings::load()
{
    QFile data(applicationName_+"_"+applicationVersion_+".txt");
    if(data.open(QFile::ReadOnly))
    {
        QString text = QString::fromLocal8Bit(data.readAll());
//        DebugLog<<text.toLocal8Bit().data()<<std::endl;
        QStringList textlist = text.split("\n");
        foreach(const QString& line, textlist)
        {
            QStringList list = line.split(",");
            if(list.size() < 3)
            {
                continue;
            }
            int type = list[0].toInt();
            switch (type) {
            case (int)QVariant::Bool:
                write(list[1],(bool)list[2].toInt());
                break;
            case (int)QVariant::Int:
                write(list[1],list[2].toInt());
                break;
            case (int)QVariant::Double:
                write(list[1],list[2].toDouble());
                break;
            case (int)QVariant::String:
                write(list[1],list[2]);
                break;
            case (int)QVariant::StringList:
            {
                QStringList tmp;
                for(int i=2; i <list.size(); i++)
                {
                    tmp.append(list[i]);
//                    DebugLog<<list[i].toLocal8Bit().data()<<std::endl;
                }
                write(list[1],tmp);
            }
                break;
            default:
                break;
            }
        }
    }
    else
    {
        DebugLog<<"Open setting file failed~!"<<std::endl;
    }

}

void Settings::save()
{
    QFile data(applicationName_+"_"+applicationVersion_+".txt");
    if(data.open(QFile::WriteOnly|QFile::Truncate))
    {
        QTextStream stream(&data);
        QMap<QString, QVariant>::const_iterator i = setting_map_.constBegin();
        while (i != setting_map_.constEnd())
        {
            switch (i.value().type())
            {
            case QVariant::Bool:
                stream<<(int)QVariant::Bool<<","<<i.key()<<","<<i.value().toBool()<<"\n";
                break;
            case QVariant::Int:
                stream<<(int)QVariant::Int<<","<<i.key()<<","<<i.value().toInt()<<"\n";
                break;
            case QVariant::Double:
                stream<<(int)QVariant::Double<<","<<i.key()<<","<<i.value().toDouble()<<"\n";
                break;
            case QVariant::String:
                stream<<(int)QVariant::String<<","<<i.key()<<","<<i.value().toString()<<"\n";
                break;
            case QVariant::StringList:
            {
                stream<<(int)QVariant::StringList<<","<<i.key();
                QStringList list = i.value().toStringList();
                foreach (const QString& str, list) {
                    stream<<","<<str;
                }
                stream<<"\n";
            }
                break;
            default:
                break;
            }

            ++i;
        }
        data.close();
    }
    else
    {
        DebugLog<<"Open setting file failed~!"<<std::endl;
    }
}



void Settings::write(const QString &_key, const bool _value)
{
    write_template(setting_map_,_key,_value);
}

void Settings::write(const QString &_key, const double _value)
{
    write_template(setting_map_,_key,_value);
}

void Settings::write(const QString &_key, const int _value)
{
    write_template(setting_map_,_key,_value);

}

void Settings::write(const QString &_key, const QString &_value)
{
    write_template(setting_map_,_key,_value);
}

void Settings::write(const QString &_key, const QStringList &_value)
{
    write_template(setting_map_,_key,_value);
}

bool Settings::read(const QString &_key,  bool& _value)
{
    if(setting_map_.count(_key))
    {
        QVariant val = setting_map_.value(_key);
        _value = val.toBool();
        return true;
    }
    else
    {
        return false;
    }
}

bool Settings::read(const QString &_key, double &_value)
{
    if(setting_map_.count(_key))
    {
        QVariant val = setting_map_.value(_key);
        _value = val.toDouble();
        return true;
    }
    else
    {
        return false;
    }
}

bool Settings::read(const QString &_key, int &_value)
{
    if(setting_map_.count(_key))
    {
        QVariant val = setting_map_.value(_key);
        _value = val.toInt();
        return true;
    }
    else
    {
        return false;
    }
}

bool Settings::read(const QString &_key, QString &_value)
{
    if(setting_map_.count(_key))
    {
        QVariant val = setting_map_.value(_key);
        _value = val.toString();
        return true;
    }
    else
    {
        return false;
    }
}

bool Settings::read(const QString &_key, QStringList &_value)
{
    if(setting_map_.count(_key))
    {
        QVariant val = setting_map_.value(_key);
        _value = val.toStringList();
        return true;
    }
    else
    {
        return false;
    }
}

Settings *Settings::getInstance()
{
    if(!settings_)
    {
        settings_ = new Settings("GCL_Mesh","0.1");
    }
    return settings_;
}



}


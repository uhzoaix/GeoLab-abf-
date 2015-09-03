#ifndef GCL_SETTINGS_H
#define GCL_SETTINGS_H
#include "Core/gcl_global.h"
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QMap>
namespace GCL{
/**
 * @brief The Settings class
 * a convenience class to load and store settings.
 *
 *
 *
 * @author Shiwei
 * @date Nov 25, 2014
 */

class GCL_CORE_SHARED_EXPORT Settings
{
public:
    Settings(const QString& _applicationName, const QString& _applicationVersion);
    virtual ~Settings();

    void load();

    void save();

    //// write
    void write(const QString& _key, const bool _value);
    void write(const QString& _key, const double _value);
    void write(const QString& _key, const int _value);
    void write(const QString& _key, const QString& _value);
    void write(const QString& _key, const QStringList& _value);

    //// read - return false if no value was found for the key
    bool read(const QString& _key, bool &_value);
    bool read(const QString& _key,  double& _value);
    bool read(const QString& _key,  int& _value);
    bool read(const QString& _key,  QString& _value);
    bool read(const QString& _key,  QStringList& _value);


    static Settings* getInstance();
  private:
    template <typename T>
    void write_template(QMap<QString,QVariant>& setting_map, const QString& _key, const T& _value)
    {
        if(setting_map.count(_key))
        {
            setting_map[_key] = QVariant(_value);
        }
        else
        {
            setting_map.insert(_key,QVariant(_value));
        }
    }
    const QString applicationName_;
    const QString applicationVersion_;
    QMap<QString, QVariant> setting_map_;

    static Settings* settings_;
};


}
#endif // SETTINGS_H

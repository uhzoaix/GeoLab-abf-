#ifndef NORMALPLUGININTERFACE_H
#define NORMALPLUGININTERFACE_H
#include "Core/gcl_global.h"
#include "PluginInterface.h"
namespace GCL { namespace Plugins {
class GCL_PLUGIN_SHARED_EXPORT NormalPluginInterface : public PluginInterface
{
    Q_OBJECT
public:
    explicit NormalPluginInterface(QObject* parent = 0);
    int getType() const {return PLUGIN_NORMAL;}
    virtual QString getCategory() const {return "Normal";}




};
}}

#define GCL_NORMALPLUGIN_INTERFACE_IID "gcl.mesh.normalplugininterface/1.0"
Q_DECLARE_INTERFACE(GCL::Plugins::NormalPluginInterface,
                    GCL_NORMALPLUGIN_INTERFACE_IID)
#endif // NORMALPLUGININTERFACE_H

#ifndef IMAGEIOINTERFACE_H
#define IMAGEIOINTERFACE_H
#include "PluginInterface.h"
namespace GCL {
class DepthImage;
namespace Plugins {
class GCL_PLUGIN_SHARED_EXPORT ImageIOInterface : public IOInterface
{
    Q_OBJECT
public:
    ImageIOInterface(QObject* parent = 0);
    virtual ~ImageIOInterface() {}
    virtual bool importDepthImage(
            const QString& filename,
            const std::shared_ptr<DepthImage>& depth_image
            ) = 0;
    virtual bool exportDepthImage(
            const  QString& filename,
            const std::shared_ptr<DepthImage>& depth_image
            ) = 0;
};
}}
#define GCL_IMAGEIO_INTERFACE_IID "gcl.mesh.imageiointerface/1.0"
Q_DECLARE_INTERFACE(GCL::Plugins::ImageIOInterface,
                    GCL_IMAGEIO_INTERFACE_IID)
#endif // IMAGEIOINTERFACE_H

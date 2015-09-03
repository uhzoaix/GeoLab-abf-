#ifndef MESHIOINTERFACE_H
#define MESHIOINTERFACE_H
#include <QStringList>
#include "PluginInterface.h"
namespace GCL
{

namespace Plugins {
/**
 * @brief The MeshIOInterface class, meshlab-like plugin interface
 * @author Shiwei
 */
class GCL_PLUGIN_SHARED_EXPORT MeshIOInterface : public IOInterface
{
    Q_OBJECT
public:
    MeshIOInterface(QObject* parent = 0);
    virtual ~MeshIOInterface() {}

    virtual bool importMesh(
            const QString& filename,
            const std::shared_ptr<OMesh>& mesh
            ) = 0;
    virtual bool exportMesh(
            const  QString& filename,
            const std::shared_ptr<OMesh>& mesh
            ) = 0;
};
}}

#define GCL_MESHIO_INTERFACE_IID "gcl.mesh.meshiointerface/1.0"
Q_DECLARE_INTERFACE(GCL::Plugins::MeshIOInterface,
                    GCL_MESHIO_INTERFACE_IID)
#endif // MESHIOINTERFACE_H

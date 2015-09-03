#ifndef IO_BASEPLUGIN_H
#define IO_BASEPLUGIN_H

#include <QObject>
#include <PluginCore/PluginInterface/MeshIOInterface.h>

namespace GCL{ namespace Plugins {
class IO_BasePlugin : public MeshIOInterface
{
    Q_OBJECT
    GCL_MESH_PLUGIN_IID_EXPORTER(GCL_MESHIO_INTERFACE_IID)
    Q_INTERFACES(GCL::Plugins::MeshIOInterface)
public:
    explicit IO_BasePlugin(QObject *parent = 0);

    QList<Format> importFormats() const;
    QList<Format> exportFormats() const;

    virtual bool importMesh(
            const QString& filename,
            const std::shared_ptr<OMesh>& mesh
            );
    virtual bool exportMesh(
            const  QString& filename,
            const std::shared_ptr<OMesh>& mesh
            );

private:
    bool importMesh_By_OBJ_Format(const QString& filename, const std::shared_ptr<OMesh>& mesh);
    bool importMesh_By_STL_Format(const QString& filename, const std::shared_ptr<OMesh>& mesh);

};
}}
#endif // IO_BASEPLUGIN_H

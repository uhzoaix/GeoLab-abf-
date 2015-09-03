#ifndef GCL_OPENMESHNODE_H
#define GCL_OPENMESHNODE_H
#include <memory>
#include "../DataStructures/OMesh.h"
#include "GeometryNode.h"
namespace GCL{ namespace SceneGraph {

class GCL_CORE_SHARED_EXPORT OpenMeshNode : public GeometryNode
{
    Q_OBJECT

public:
    OpenMeshNode(const std::shared_ptr<OMesh>& _mesh, Node* parent = 0);
    virtual GeometryBase* getGeometry();

    virtual QString getNodeTypeName() const {return tr("OpenMeshNode");}
    std::shared_ptr<OMesh> getMesh() {return mesh_;}
    OMesh* get_mesh() {return mesh_.get();}
    void reset(const std::shared_ptr<OMesh>& _mesh) {mesh_ = _mesh;}
public slots:
    virtual void slotAction(const QString &action_name);
private:
    std::shared_ptr<OMesh> mesh_;
};
}}
#endif // OPENMESHNODE_H

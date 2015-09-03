#ifndef LIGHTMESHNODE_H
#define LIGHTMESHNODE_H
#include "GeometryNode.h"
#include "Core/DataStructures/LightMesh.h"
#include <memory>
namespace GCL {
namespace SceneGraph {
class GCL_CORE_SHARED_EXPORT LightMeshNode : public GeometryNode
{
public:
    LightMeshNode(const std::shared_ptr<LightMesh>& _mesh, Node* parent = 0);
    ~LightMeshNode();

    LightMesh* get_light_mesh() {return light_mesh_.get();}
    GeometryBase* getGeometry() {return light_mesh_.get();}
private:
    std::shared_ptr< LightMesh > light_mesh_;
};
}}
#endif // LIGHTMESHNODE_H

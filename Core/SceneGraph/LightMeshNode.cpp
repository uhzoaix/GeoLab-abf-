#include "LightMeshNode.h"


namespace GCL { namespace SceneGraph {
LightMeshNode::LightMeshNode(const std::shared_ptr<LightMesh> &_mesh, Node *parent)
    :GeometryNode(parent),light_mesh_(_mesh)
{

}

LightMeshNode::~LightMeshNode()
{

}

}}

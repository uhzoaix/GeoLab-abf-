#include "ImagePastingNode.h"
#include "Utilities/OpenMeshAlgorithms/Pasting/ImagePasting.h"
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/IO/IOInstances.hh>
#include <GCL/Core/DataStructures/OMesh.h>
namespace GCL { namespace SceneGraph {

ImagePastingNode::ImagePastingNode(const std::shared_ptr<OMesh> &_mesh, Node *parent)
    :image_pasting_(new Utilities::ImagePasting()),mesh_(_mesh), GeometryNode(parent)
{

}

ImagePastingNode::~ImagePastingNode()
{

}

GeometryBase *ImagePastingNode::getGeometry()
{
    return mesh_.get();
}

void ImagePastingNode::render(RenderModes *pRenderModes)
{
    GeometryNode::render(pRenderModes);

}

}}

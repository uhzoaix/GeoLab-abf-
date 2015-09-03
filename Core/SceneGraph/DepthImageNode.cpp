#include "DepthImageNode.h"
#include <Core/DataStructures/DepthImage.h>
#include <Core/SceneGraph/RenderModes.h>

#include "qgl.h"
namespace GCL {
namespace SceneGraph{



DepthImageNode::DepthImageNode(const std::shared_ptr<DepthImage> &_depth_image, Node *parent)
    :depth_image_(_depth_image),GeometryNode(parent)
{
    this->setRenderSettingsOverriding(true);
    this->getRenderModes()->set_render_mode_directly(RenderModes::VERTEX_COLOR | RenderModes::VERTEX_DRAWING);
}

GeometryBase *DepthImageNode::getGeometry()
{
    return depth_image_.get();
}

DepthImage *DepthImageNode::getDepthImage()
{
    return depth_image_.get();
}

void DepthImageNode::renderPrologue()
{
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    glDisable(GL_DEPTH_TEST);
    glPointSize(5);
}

void DepthImageNode::renderCurtainCall()
{
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_COLOR_MATERIAL);

}

}}

#ifndef GCL_DEPTHIMAGENODE_H
#define GCL_DEPTHIMAGENODE_H
#include "GeometryNode.h"
namespace GCL {
class DepthImage;
namespace SceneGraph {
class GCL_CORE_SHARED_EXPORT DepthImageNode : public GeometryNode
{
    Q_OBJECT
public:
    DepthImageNode(const std::shared_ptr<DepthImage>& _depth_image, Node* parent = 0);
    virtual QString getNodeTypeName() const {return tr("DepthImageNode");}

    GeometryBase* getGeometry();
    DepthImage* getDepthImage();

    void renderPrologue();
    void renderCurtainCall();

private:
    std::shared_ptr<DepthImage> depth_image_;
};
}}
#endif // DEPTHIMAGENODE_H

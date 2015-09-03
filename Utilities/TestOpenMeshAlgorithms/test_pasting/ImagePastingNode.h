#ifndef IMAGEPASTINGNODE_H
#define IMAGEPASTINGNODE_H
#include <GCL/Core/SceneGraph/GeometryNode.h>
#include <memory>
namespace GCL {
class OMesh;
namespace Utilities {
class ImagePasting;
}

namespace SceneGraph {
class ImagePastingNode : public GeometryNode
{
    Q_OBJECT
public:
    ImagePastingNode(const std::shared_ptr<OMesh>& _mesh,Node* parent);
    ~ImagePastingNode();


    Utilities::ImagePasting*  getImagePasting() {return image_pasting_.get();}

    GeometryBase*  getGeometry();

    void render(RenderModes *pRenderModes);


private:
    std::shared_ptr<Utilities::ImagePasting> image_pasting_;
    std::shared_ptr<OMesh> mesh_;

};
}}
#endif // IMAGEPASTINGNODE_H

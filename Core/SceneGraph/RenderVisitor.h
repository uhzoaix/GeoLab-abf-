#ifndef GCL_RENDERVISITOR_H
#define GCL_RENDERVISITOR_H
#include "Core/gcl_global.h"
#include "Visitor.h"
#include "Renderer.h"
#include <vector>
namespace GCL { namespace SceneGraph {
class RenderModes;
class GCL_CORE_SHARED_EXPORT RenderVisitor : public Renderer , public Visitor
{
public:
    RenderVisitor();
    ~RenderVisitor() {}


    //// rendering
    virtual void render( SceneGraphNode* _scenegraph );

    virtual bool enter(SceneGraphNode * _sceneGraphNode);
    virtual void leave(SceneGraphNode * _sceneGraphNode);

    virtual bool enter(GroupNode * _groupNode);
    virtual void leave(GroupNode * _groupNode);

    virtual bool enter(GeometryNode * _geometryNode);
    virtual void leave(GeometryNode * _geometryNode);

    virtual bool enter(TransformationNode * _transformationNode);
    virtual void leave(TransformationNode * _transformationNode);
private:
    void renderPrologue();

    /// Use a stack to save the active renderModes during visiting the scene tree
    std::vector<RenderModes * > renderModesStack_;


};
}}
#endif // RENDERVISITOR_H

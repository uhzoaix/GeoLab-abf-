#ifndef GCL_ZBUFFERRENDERER_H
#define GCL_ZBUFFERRENDERER_H
#include "gcl_global.h"
#include <memory>
#include "../RenderVisitor.h"
namespace GCL{ namespace SceneGraph {
/**
      Reference: Dolphin Prefilter
      The ZBufferRenderer renders a scene and stores the created Depth Buffer
      image for later retriaval in Processor Memory.

**/
class GCL_CORE_SHARED_EXPORT ZBufferRenderer : public RenderVisitor
{
public:
    /**
    Creates a new ZBufferRenderer.
    */
    ZBufferRenderer();
    ~ZBufferRenderer() {}
    //// rendering
    /**
    Renders the passed Scenegraph with settings that make the ZBuffer
    usefull for later usage. Reads out the depth buffer and stores it in
    an internal buffer.
    \param _sceneGraph  A Scenegraph
    */
    void render(SceneGraphNode* _sceneGraph);

    //// Querying
    /**
    Returns a ZBuffer bitmap. Only call after one was created by
    render.
    \return an immage of the depth buffer of the scene
    */
    std::shared_ptr<float> getZBuffer() const;

    /**
    Returns the viewport at time of rendering.
    \param _vp  A pointer to a 4 element array
    */
    void getViweport(int * _vp) const;


private:
    std::shared_ptr<float> zBuff_;
    int vp_[4];

};
}}
#endif // ZBUFFERRENDERER_H

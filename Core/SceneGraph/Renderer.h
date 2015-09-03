#ifndef GCL_RENDER_H
#define GCL_RENDER_H
#include "Core/gcl_global.h"
namespace GCL { namespace SceneGraph {


class SceneGraphNode;
class GCL_CORE_SHARED_EXPORT Renderer{
    public:
        virtual void render( SceneGraphNode* _scenegraph) = 0;
};


}}
#endif // RENDER_H

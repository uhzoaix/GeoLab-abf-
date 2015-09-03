#include "ZBufferRenderer.h"
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include "../SceneGraph.h"
#include "../GeometryNode.h"
#include "../GroupNode.h"
#include "../Lights.h"
#include "../Camera.h"
#include "../../Utilities/LogSystem.h"
namespace GCL{ namespace SceneGraph {

ZBufferRenderer::ZBufferRenderer()
{

}

void ZBufferRenderer::render(SceneGraphNode *_sceneGraph)
{
    glShadeModel(GL_SMOOTH);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);

    _sceneGraph->accept(*this); // Render the scenegraph
    glGetIntegerv(GL_VIEWPORT, vp_);
    zBuff_ = std::shared_ptr<float>(new float[abs(vp_[2]*vp_[3])]);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(vp_[0], vp_[1], vp_[2], vp_[3],  GL_DEPTH_COMPONENT, GL_FLOAT, zBuff_.get());
}

std::shared_ptr<float> ZBufferRenderer::getZBuffer() const
{
    return zBuff_;
}

void ZBufferRenderer::getViweport(int *_vp) const
{
    _vp[0] = vp_[0];
    _vp[1] = vp_[1];
    _vp[2] = vp_[2];
    _vp[3] = vp_[3];
}



}}

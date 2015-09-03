#include "RenderVisitor.h"
#include "SceneGraph.h"
#include "GeometryNode.h"
#include "GroupNode.h"
#include "Lights.h"
#include "Camera.h"
#include "../Utilities/LogSystem.h"
#ifdef  _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include "GLStates.h"
#include "TransformationNode.h"
#include "Shader.h"
namespace GCL { namespace SceneGraph {

RenderVisitor::RenderVisitor()
{
}

void RenderVisitor::render(SceneGraphNode *_scenegraph)
{
    this->renderPrologue();

    _scenegraph->accept(*this);
}

bool RenderVisitor::enter(SceneGraphNode *_sceneGraphNode)
{
    if(!_sceneGraphNode->visible())
    {
        return false;
    }
    renderModesStack_.clear();
    renderModesStack_.push_back(_sceneGraphNode->getRenderModes());
    _sceneGraphNode->getGLState()->updateGL();
    _sceneGraphNode->getShader()->enter();
    _sceneGraphNode->getCamera()->enter();
    _sceneGraphNode->getLights()->updateGL();

    return true;
}

void RenderVisitor::leave(SceneGraphNode *_sceneGraphNode)
{
    if(!_sceneGraphNode->visible())
    {
        return;
    }
    renderModesStack_.clear();
    _sceneGraphNode->renderDecorations(true);
    _sceneGraphNode->getCamera()->leave();
    _sceneGraphNode->getShader()->leave();
    _sceneGraphNode->renderDecorations(false);

}

bool RenderVisitor::enter(GroupNode *_groupNode)
{
    if(!_groupNode->visible()) return false;
    if(_groupNode->isRenderSettingsOverriding())
    {
        renderModesStack_.push_back(_groupNode->getRenderModes());
    }
    return true;
}

void RenderVisitor::leave(GroupNode *_groupNode)
{
    if(!_groupNode->visible()) return;
    if(_groupNode->isRenderSettingsOverriding())
    {
        renderModesStack_.pop_back();
    }
}

bool RenderVisitor::enter(GeometryNode *_geometryNode)
{
    if(!_geometryNode->visible()) return false;
    if(_geometryNode->isRenderSettingsOverriding())
    {

        renderModesStack_.push_back(_geometryNode->getRenderModes());
    }
    _geometryNode->render(renderModesStack_.back());
    return true;
}

void RenderVisitor::leave(GeometryNode *_geometryNode)
{
    if(!_geometryNode->visible()) return;
    if(_geometryNode->isRenderSettingsOverriding())
    {
        renderModesStack_.pop_back();
    }
}

bool RenderVisitor::enter(TransformationNode *_transformationNode)
{
    _transformationNode->render_enter();
    return true;
}

void RenderVisitor::leave(TransformationNode *_transformationNode)
{
    _transformationNode->render_leave();
}



void RenderVisitor::renderPrologue()
{
    glShadeModel(GL_SMOOTH);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(2);

}



}}

#include "CtrlPointsNode.h"
#include "RenderModes.h"
#include "Material.h"
#include "../DataStructures/GeometryBase.h"
#include <qgl.h>
//#include <GL/glut.h>
namespace GCL { namespace SceneGraph {

CtrlPointsNode::CtrlPointsNode(Node *parent)
    :GeometryNode(parent)
{

    setRenderSettingsOverriding(true);
    getRenderModes()->set_render_mode_directly(RenderModes::VERTEX_DRAWING | RenderModes::VERTEX_SELECTION);
    getMaterial()->setPointColor(0.6f,0.6f,0.f,1.f,false);
    setSelectionMode(SelectionMode(true,false,false));
}

CtrlPointsNode::~CtrlPointsNode()
{
}

void CtrlPointsNode::render(RenderModes *pRenderModes)
{
    glEnable(GL_COLOR_MATERIAL);
    glDisable(GL_BLEND);
    if(pRenderModes->has_render_mode(RenderModes::VERTEX_DRAWING))
    {
        for(size_t i=0; i < getGeometry()->getPointsNumber(); i++)
        {
            Vec3 v = getGeometry()->getVertexPosition(i);
            if(pRenderModes->has_render_mode(RenderModes::VERTEX_SELECTION) && getGeometry()->isVertexSelected(i))
            {

                glColor4fv(getMaterial()->getSelectionColor4fv());

            }
            else
            {
                glColor4fv(getMaterial()->getPointColor4fv());
            }
            glPushMatrix();
            glTranslated(v[0],v[1],v[2]);
//            glutSolidSphere(0.001 * getMaterial()->getVertexRadius(),60,20);
            glPopMatrix();
        }
    }
    glDisable(GL_COLOR_MATERIAL);
    glEnable(GL_BLEND);
}


}}

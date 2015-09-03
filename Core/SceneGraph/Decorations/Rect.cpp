#include "Rect.h"
#include "qgl.h"
namespace GCL { namespace Decoration {

Rect::Rect()
{
    this->setName("Decoration:Rect");
    this->set_with_camera(false);
}

void Rect::render()
{
    float x0,y0,x1,y1;
    x0 = -1 + 2 * v_[0];
    y0 = -1 + 2 * v_[1];
    x1 = -1 + 2 * v_[2];
    y1 = -1 + 2 * v_[3];
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(0.8f,0.2f,0.2f,0.6f);
    glBegin(GL_QUADS);

    glVertex2d(x0,y0);
    glVertex2d(x0,y1);

    glVertex2d(x1,y1);
    glVertex2d(x1,y0);
    glEnd();
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}
}}

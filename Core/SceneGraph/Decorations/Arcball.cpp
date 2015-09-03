#include "Arcball.h"
#include <qgl.h>
#include "GL/glu.h"
namespace GCL { namespace Decoration {



void glDrawCircle(double radius, int side = 60)
{
    int nside = side;
    const double pi2 = 3.14159265 * 2.0;
    glBegin (GL_LINE_LOOP);
    for (double i = 0; i < nside; i++) {
      glNormal3d (cos (i * pi2 / nside), sin (i * pi2 / nside), 0.0);
      glVertex3d (cos (i * pi2 / nside) * radius, sin (i * pi2 / nside) * radius, 0.0);
    }
    glEnd ();
}


Arcball::Arcball()
{
    this->setName("Decoraion:Arcball");

}

void Arcball::render()
{
    glPushAttrib(GL_TRANSFORM_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | GL_LINE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT);

    glPushMatrix();
    glTranslatef(-offset_[0],-offset_[1],-offset_[2]);
    glDepthMask(GL_FALSE);
    float amb[4] = { .35f, .35f, .35f, 1.0f };
    float col[4] = { .5f, .5f, .8f, 1.0f };
    glEnable (GL_LINE_SMOOTH);
    glDisable(GL_COLOR_MATERIAL); // has to be disabled, it is used by wrapper to draw meshes, and prevent direct material setting, used here
    glEnable (GL_LIGHTING);
    glEnable (GL_LIGHT0);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glMaterialfv (GL_FRONT_AND_BACK, GL_EMISSION, amb);
    glLineWidth(2);
    col[0] = 0.f; col[1] = 0.f; col[2] = 1.f;
    float len = radius_;
    glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, col);
    glDrawCircle(len);

    glRotatef (90, 1, 0, 0);
    col[0] = 1.00f; col[1] = 0.f; col[2] = .00f;
    glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, col);
    glDrawCircle(len);

    glRotatef (90, 0, 1, 0);
    col[0] = 0.f; col[1] = 1.f; col[2] = 0.f;
    glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, col);
    glDrawCircle(len);
    glPopMatrix();
    glPopAttrib ();
}


}}

#include "GLStates.h"
#include "qgl.h"

namespace GCL { namespace SceneGraph {
GLStates::GLStates():lighting_(true),depth_test_(true),color_material_(false),blend_(true),
    background_color_bottom_(1,1,1,1),background_color_top_(1,1,1,1)
{

}

GLStates::~GLStates()
{

}

void GLStates::updateGL()
{
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glBegin(GL_QUADS);
    glColor4dv(background_color_bottom().data());

    glVertex2d(-1.0,-1.0);
    glVertex2d(1.0,-1.0);
    glColor4dv(background_color_top().data());

    glVertex2d(1.0,1.0);
    glVertex2d(-1.0,1.0);
    glEnd();

//    glColor3d(0.3f,0.5f,0.5f);


    if(lighting_)
    {
        glEnable(GL_LIGHTING);
    }
    else
    {
        glDisable(GL_LIGHTING);
    }

    if(blend_)
    {
        glEnable(GL_BLEND);
    }
    else
    {
        glDisable(GL_BLEND);
    }

    if(color_material_)
    {
        glEnable(GL_COLOR_MATERIAL);
    }
    else
    {
        glDisable(GL_COLOR_MATERIAL);
    }
    if(depth_test_)
    {
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }





}
bool GLStates::lighting() const
{
    return lighting_;
}

void GLStates::setLighting(bool lighting)
{
    lighting_ = lighting;
}
bool GLStates::depth_test() const
{
    return depth_test_;
}

void GLStates::setDepth_test(bool depth_test)
{
    depth_test_ = depth_test;
}
bool GLStates::color_material() const
{
    return color_material_;
}

void GLStates::setColor_material(bool color_material)
{
    color_material_ = color_material;
}
bool GLStates::blend() const
{
    return blend_;
}

void GLStates::setBlend(bool blend)
{
    blend_ = blend;
}
Vec4 GLStates::background_color_top() const
{
    return background_color_top_;
}

void GLStates::setBackground_color_top(const Vec4 &background_color_top)
{
    background_color_top_ = background_color_top;
}
Vec4 GLStates::background_color_bottom() const
{
    return background_color_bottom_;
}

void GLStates::setBackground_color_bottom(const Vec4 &background_color_bottom)
{
    background_color_bottom_ = background_color_bottom;
}







}}

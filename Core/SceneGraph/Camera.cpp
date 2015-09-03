#include "Camera.h"
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
namespace GCL{ namespace SceneGraph {

Camera::Camera()
    :pos_(1,0,0),center_(0,0,0),up_(0,0,1)
{
}

void Camera::enter()
{
    set_viewport(width_,height_);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();
    gluLookAt(pos_[0],pos_[1],pos_[2], center_[0],center_[1],center_[2],up_[0],up_[1],up_[2]);
    glMultMatrixd(rotate_matrix_.data());
    glTranslated(offset_[0],offset_[1],offset_[2]);

    update_viewport_and_matrix();
}

void Camera::leave()
{
    glPopMatrix();
}



void Camera::set_viewport(int w, int h)
{

    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    Scalar dist = (pos_ - center_).length();
    gluPerspective(45.0,GLdouble(w)/GLdouble(h),0.1,dist * 5);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    width_ = w;
    height_ = h;
}

void Camera::get_project_ray(int posx, int posy, Vec3 &ray_start, Vec3 &ray_direction) const
{
    GLdouble x,y,z;
    gluUnProject((GLdouble)posx,(GLdouble)posy,0.2,model_matrix_.data(), project_matrix_.data(), viewport_.data(),&x,&y,&z);
    ray_start = Vec3(x,y,z);
    gluUnProject((GLdouble)posx,(GLdouble)posy,1.0,model_matrix_.data(), project_matrix_.data(), viewport_.data(),&x,&y,&z);
    ray_direction = Vec3(x,y,z) - ray_start;
}

Vec3 Camera::get_camera_position() const
{
    return center() + get_direction() * distance() - offset_;
}

Vec3 Camera::getCameraPosition(int posx, int posy) const
{
    GLdouble x,y,z;
    gluUnProject((GLdouble)posx,(GLdouble)posy,0.0,model_matrix_.data(), project_matrix_.data(), viewport_.data(),&x,&y,&z);
    Vec3 ray_start = Vec3(x,y,z);
//    gluUnProject((GLdouble)posx,(GLdouble)posy,1.0,model_matrix_.data(), project_matrix_.data(), viewport_.data(),&x,&y,&z);
//    Vec3 ray_direction = Vec3(x,y,z) - ray_start;
    return ray_start;
}

Vec3 Camera::get_direction() const
{
    Quat quat;
    quat.convertFromMatrix(model_matrix_);
    HomoMatrix4 inverseModelMatrix = quat.conjugate().convertToMatrix();
    return  (inverseModelMatrix * up_).normalize();
}

Scalar Camera::distance() const
{
    return (pos_ - center_).length();
}



void Camera::update_viewport_and_matrix()
{
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT,viewport);
    viewport_ = Vec4i(viewport);
    double projMatrix[16];
    glGetDoublev(GL_PROJECTION_MATRIX,projMatrix);
    project_matrix_ = HomoMatrix4(projMatrix);
    double modelMatrix[16];
    glGetDoublev(GL_MODELVIEW_MATRIX,modelMatrix);
    model_matrix_ = HomoMatrix4(modelMatrix);
}

}}

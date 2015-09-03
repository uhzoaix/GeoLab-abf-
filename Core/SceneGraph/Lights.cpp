#include "Lights.h"


#ifdef _WIN32
#include<windows.h>
#endif
#include <GL/gl.h>


namespace GCL{ namespace SceneGraph {

Light::Light(const int _glIndex)
    :glIndex_(_glIndex),
      isAttachedToCamera_(false),
      enabled_(true),
      position_(0.f),
      ambient_(0.f),
      diffuse_(0.f),
      specular_(0.f)
{

}

Light::~Light()
{

}

void Light::updateGL()
{
    if(enabled_) {
        glEnable(GL_LIGHT0 + glIndex_);
    }
    else {
        glDisable(GL_LIGHT0 + glIndex_);
    }

    //Position
    if(isAttachedToCamera_) {
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        glLightfv(GL_LIGHT0 + glIndex_, GL_POSITION, position_.data());
        glPopMatrix();
    }
    //Setting Colors
    glLightfv(GL_LIGHT0 + glIndex_, GL_AMBIENT,  ambient_.data());
    glLightfv(GL_LIGHT0 + glIndex_, GL_DIFFUSE,  diffuse_.data());
    glLightfv(GL_LIGHT0 + glIndex_, GL_SPECULAR, specular_.data());
}


//==Lights==========================================================

Lights::Lights() {
    //init data
    max_lights_number = GL_MAX_LIGHTS;
}

Lights::~Lights() {
}


void Lights::setDefaultLights() {

    Light* light1 = addLight();
    light1->setPosition(Vec3f(0.0, 0.0, 1.0));
    light1->setDiffuse(Vec4f(.6f,.4f,.2f,1.f));
    light1->setSpecular(Vec4f(.6f,.4f,.2f,1.f));
    light1->setAttachedToCamera(true);
    light1->setEnabled(true);

    Light* light0 = addLight();
    light0->setAmbient(Vec4f(.05f, .1f, .15f,1.f));
    light0->setEnabled(true);

    Light* light2 = addLight();
    light2->setPosition(Vec3f(-1.f,  1.f, 0.7f));
    light2->setDiffuse(Vec4f(.1f, .4f, .6f,1.f));
    light2->setAttachedToCamera(false);
    light2->setEnabled(true);

}

void Lights::updateGL()
{
    for(std::vector<std::shared_ptr<Light> >::iterator it=lights_.begin(); it!=lights_.end();++it){
        (*it)->updateGL();
    }
}


//0 if not enough lights
Light *Lights::addLight() {
    if((int)lights_.size()==max_lights_number) {
        return 0;
    }
    else {
        std::shared_ptr<Light> light(new Light((int)lights_.size()));
        lights_.push_back(light);
        return light.get();
    }
}



}}

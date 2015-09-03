#ifndef GCL_SCENEGRAPH_LIGHTS_H
#define GCL_SCENEGRAPH_LIGHTS_H
#include "gcl_global.h"
#include "../Math/MathDefines.h"
#include <memory>
#include <vector>
namespace GCL{ namespace SceneGraph {
/**
    This Class represents the colors and position a OpenGL light as defined in:
    http://www.opengl.org/sdk/docs/man/xhtml/glLight.xml
    Additionally a light can be attached to a camera and dis/enabled.
    This Class is meant to be used with the following class Lights.
*/
class GCL_CORE_SHARED_EXPORT Light {
public:

    explicit Light(const int _glIndex);
    virtual ~Light();

    void updateGL();

    void setEnabled(const bool _enable){ enabled_=_enable; }
    bool isEnabled() const{ return enabled_; }

    inline int getGLIndex() const { return glIndex_; }

    inline bool isAttachedToCamera() const { return isAttachedToCamera_; }
    void setAttachedToCamera(const bool _attach){ isAttachedToCamera_ = _attach; }

    void setPosition(const Vec3f& p ){ for(int i=0;i<3;i++){ position_[i]=(float)p[i]; } position_[3]=0.f; }
    void setPosition( const Vec4f& p ){ position_ = p;  }
    Vec4f getPosition() const {return position_;}
    const float* getPosition4fv() const { return position_.data(); }

    void setAmbient(const Vec4f& p ) { ambient_ = p;  }
    Vec4f getAmbient() const {return ambient_;}
    inline const float* getAmbient4fv() const { return ambient_.data(); }


    void setDiffuse(const Vec4f& c ){diffuse_ = c;}
    Vec4f getDiffuse() const {return diffuse_;}
    inline const float* getDiffuse4fv() const { return diffuse_.data(); }

    void setSpecular(const Vec4f& c ){specular_ = c;}
    Vec4f getSpecular() const {return specular_;}
    inline const float*  getSpecular4fv() const { return specular_.data(); }

private:

    const int glIndex_;//GL_LIGHTi = GL_LIGHT0 + glIndex
    bool isAttachedToCamera_;//scene or eye coordinate space
    bool enabled_;

    Vec4f position_;//world coordinates if !isAttachedToCamera_, else camera coordinates
    Vec4f ambient_;
    Vec4f diffuse_;
    Vec4f specular_;

};



/**
  This Class is a container of mulitple Light(s) objects.
  Lights can be added by addLight(), as e.g. used in setDefaultLights().
  updateGL() should called whenever the lights need to be resent to opengl,
  e.g. for every widget rendering the scenegraph.
  This class is meant to be used with the class Scenegraph.
  */
class GCL_CORE_SHARED_EXPORT Lights {

public:
    typedef std::vector<std::shared_ptr<Light> > LightsVector;

public:
    explicit Lights();
    ~Lights();

    void setDefaultLights();

    /** Updates the lights in the current GL Widget/context if something changed */
    void updateGL();

    Light* addLight(); //0 if not enough lights

    LightsVector::iterator begin(){ return lights_.begin();}
    LightsVector::iterator end(){ return lights_.end();}

    void clear() {lights_.clear();}
private:
    LightsVector lights_;
    int max_lights_number;
};

}}
#endif // LIGHTS_H

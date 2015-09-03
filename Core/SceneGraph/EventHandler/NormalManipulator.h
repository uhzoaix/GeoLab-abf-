#ifndef GCL_NORMALMANIPULATOR_H
#define GCL_NORMALMANIPULATOR_H
#include "gcl_global.h"
#include "Manipulator.h"
#include "../../Math/MathDefines.h"
namespace GCL { namespace SceneGraph {

class Camera;
class GCL_CORE_SHARED_EXPORT NormalManipulator : public Manipulator
{
public:
    NormalManipulator(SceneGraphNode* _scenegraph);
    const char* getEventHandlerName() const {return "NormalManipulator";}
    virtual ~NormalManipulator();
    virtual int resize(int w, int h);
    virtual int mouseMove(int posX, int posY);
    virtual int mousePress(int posX,int posY, unsigned int button, unsigned int modifier = NoModifier);
    virtual int mouseRelease(int posX, int posY, unsigned int button, unsigned int modifier = NoModifier) ;
    virtual int keyUp(const unsigned int& key, unsigned int modifier = NoModifier);
    virtual int keyDown(const unsigned int& key, unsigned int modifier = NoModifier);
    virtual int wheel(const float& delta);

    void OrientTop();
    void OrientBottom();
    void OrientRight();
    void OrientLeft();
    void OrientFront();
    void OrientBack();
    void OrientNormal();
    virtual void focus(const Vec3 &center, const Scalar &radius);
protected:
    void    updateCamera();
    void    updateRotation();
    virtual void rotate(int posX, int posY);

    virtual void  pan(int posX, int posY);
    void    zoom(const float &delta);
    double mDistance;
    double mTheta;
    double mPhi;
    double mAngle;
    Vec3 mDefaultCenter;
    float mDefaultDistance;
    int mCurrentX;
    int mCurrentY;
    int mUpDirection;


};
}}
#endif // ARCBALLMANIPULATOR_H

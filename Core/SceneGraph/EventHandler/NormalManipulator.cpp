#include "NormalManipulator.h"
#include "../SceneGraph.h"
#include "../Decorations/Arcball.h"
#include "../Camera.h"
#include "../../Math/MathDefines.h"
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
namespace GCL { namespace SceneGraph {




NormalManipulator::NormalManipulator(SceneGraphNode *_scenegraph)
    :Manipulator(_scenegraph)
{
    mDefaultDistance = 5;
    this->OrientNormal();
}

NormalManipulator::~NormalManipulator()
{
}

int NormalManipulator::resize(int w, int h)
{
    return 0;
}

int NormalManipulator::mouseMove(int posX, int posY)
{

    EventHandler::mouseMove(posX,posY);
    if(getModifier() != NoModifier) return 0;
    if(getButton() == LeftButton)
    {
        this->rotate(posX,posY);
    }
    else if(getButton() == RightButton)
    {
        this->pan(posX,posY);
    }
    mCurrentX = posX;
    mCurrentY = posY;
    return 0;
}

int NormalManipulator::mousePress(int posX, int posY, unsigned int button, unsigned int modifier)
{
    EventHandler::mousePress(posX,posY,button,modifier);
    if(getModifier() != NoModifier) return 0;
    mCurrentX = posX;
    mCurrentY = posY;



    return 0;
}

int NormalManipulator::mouseRelease(int posX, int posY, unsigned int button, unsigned int modifier)
{
    EventHandler::mouseRelease(posX,posY,button,modifier);
    if(getModifier() != NoModifier) return 0;
    if(button == LeftButton)
    {
        this->rotate(posX,posY);
    }

    //    The position (0,0) in OpenGL on Screen is at LeftBottom, it's diffrent with the common postion at LeftTop;
    //    posY = this->mHeight - posY;
    //    mCamera->testProjectRay(posX,posY);
    return 0;
}

int NormalManipulator::keyUp(const unsigned int &key, unsigned int modifier)
{
    if(modifier) return 0;
    switch (key) {
    case Key_W:
        this->OrientTop();
        break;
    case Key_S:
        this->OrientBottom();
        break;
    case Key_F:
        this->OrientFront();
        break;
    case Key_B:
        this->OrientBack();
        break;
    case Key_A:
        this->OrientLeft();
        break;
    case Key_D:
        this->OrientRight();
        break;
    case Key_P:
        this->set_show_arcball(!is_show_arcball());
        break;
    case Key_Up:
        this->wheel(0.1f);
        break;
    case Key_Down:
        this->wheel(-0.1f);
        break;
    case Key_Space:
        this->OrientNormal();
        break;
    default:
        break;
    }
    return 0;
}

int NormalManipulator::keyDown(const unsigned int &key, unsigned int modifier)
{
    return 0;
}

int NormalManipulator::wheel(const float &delta)
{
    if(delta > 0)
    {
        this->zoom(0.1f);
    }
    else
    {
        this->zoom(-0.1f);
    }
    return 0;
}
void NormalManipulator::OrientNormal()
{
    getSceneGraph()->getCamera()->set_center(mDefaultCenter);
    getSceneGraph()->getCamera()->set_default_offset();

    this->mTheta = -M_PI / 4;
    this->mPhi = M_PI / 4;
    this->mDistance  = this->mDefaultDistance;

    this->updateRotation();
    this->updateCamera();
}

void NormalManipulator::focus(const Vec3 &center, const Scalar &radius)
{
    getSceneGraph()->getCamera()->move(center - getSceneGraph()->getCamera()->getOffset());
    mDistance = radius;
    this->updateCamera();
}

void NormalManipulator::OrientTop()
{
    getSceneGraph()->getCamera()->set_center(mDefaultCenter);
    getSceneGraph()->getCamera()->set_default_offset();
    this->mTheta =  -M_PI / 2;
    this->mPhi = ZERO_TOLERANCE;
    this->mDistance = this->mDefaultDistance;
    this->updateRotation();
    this->updateCamera();

}

void NormalManipulator::OrientBottom()
{
    getSceneGraph()->getCamera()->set_center(mDefaultCenter);
    getSceneGraph()->getCamera()->set_default_offset();
    this->mTheta = -M_PI / 2;
    this->mPhi = M_PI - ZERO_TOLERANCE;
    this->mDistance = this->mDefaultDistance;
    this->updateRotation();
    this->updateCamera();
}
void NormalManipulator::OrientLeft()
{
    getSceneGraph()->getCamera()->set_center(mDefaultCenter);
    getSceneGraph()->getCamera()->set_default_offset();
    this->mTheta = -M_PI / 2;
    this->mPhi = M_PI / 2;
    this->mDistance = this->mDefaultDistance;
    this->updateRotation();
    this->updateCamera();
}
void NormalManipulator::OrientRight()
{
    getSceneGraph()->getCamera()->set_center(mDefaultCenter);
    getSceneGraph()->getCamera()->set_default_offset();
    this->mTheta = M_PI / 2;
    this->mPhi = M_PI / 2;
    this->mDistance = this->mDefaultDistance;
    this->updateRotation();
    this->updateCamera();
}
void NormalManipulator::OrientFront()
{
    getSceneGraph()->getCamera()->set_center(mDefaultCenter);
    getSceneGraph()->getCamera()->set_default_offset();
    this->mTheta = 0 ;
    this->mPhi = M_PI / 2;
    this->mDistance = this->mDefaultDistance;
    this->updateRotation();
    this->updateCamera();
}
void NormalManipulator::OrientBack()
{
    getSceneGraph()->getCamera()->set_center(mDefaultCenter);
    getSceneGraph()->getCamera()->set_default_offset();
    this->mTheta = M_PI;
    this->mPhi = M_PI / 2;
    this->mDistance = this->mDefaultDistance;
    this->updateRotation();
    this->updateCamera();
}
void NormalManipulator::updateCamera()
{
    Vec3 v0 = getSceneGraph()->getCamera()->position() - getSceneGraph()->getCamera()->center();
    v0.Normalize();
    getSceneGraph()->getCamera()->set_position(getSceneGraph()->getCamera()->center() + v0 * mDistance);
    arcball_->setRadius(mDistance / 4.0);
    arcball_->setOffset(getSceneGraph()->getCamera()->getOffset());
}

void NormalManipulator::updateRotation()
{
    Vec3 v0 = getSceneGraph()->getCamera()->position() - getSceneGraph()->getCamera()->center();
    v0.Normalize();

    Vec3 v1(cos(mTheta) * sin(mPhi),sin(mTheta) * sin(mPhi),cos(mPhi));
    getSceneGraph()->getCamera()->rotation_matrix() = (Quat::quatFromVectorTransform(v1,v0).convertToMatrix());

}

void NormalManipulator::rotate(int posX, int posY)
{
    Vec3 hitOld(cos(mTheta) * cos(mPhi), sin(mTheta) * cos(mPhi), sin(mPhi));

    Vec4i viewport = getSceneGraph()->getCamera()->get_viewport();
    float side =  -(float)(posX - mCurrentX) / (float)viewport[2] * 2.f;
    float updown =   -(float)(posY - mCurrentY) / (float)viewport[3] * 2.f;
    mTheta += side * 4.f;
    mPhi += updown * 4.f;
    while(mTheta > M_PI)
    {
        mTheta -= 2 * M_PI;
    }
    while(mTheta < -M_PI)
    {
        mTheta += 2 * M_PI;
    }
    while(mPhi > 2 * M_PI)
    {
        mPhi  -= 2 * M_PI;
    }
    while(mPhi < 0 )
    {
        mPhi += 2 * M_PI;
    }
    //    if(mPhi > M_PI && mUpDirection > 0)
    //    {
    //        getSceneGraph()->getCamera()->setUp(Vec3() - getSceneGraph()->getCamera()->up());
    //        mUpDirection = -1;
    //    }
    //    if(mPhi < M_PI && mUpDirection < 0)
    //    {
    //        getSceneGraph()->getCamera()->setUp(Vec3() - getSceneGraph()->getCamera()->up());
    //        mUpDirection = 1;
    //    }
    Vec3 hitNew(cos(mTheta) * cos(mPhi), sin(mTheta) * cos(mPhi), sin(mPhi));
    Vec3 center = getSceneGraph()->getCamera()->center();
    Vec3 axis = (hitOld - center) ^ (hitNew - center);
    if(axis.Normalize() < ZERO_TOLERANCE) return;
    Scalar angle = (hitNew - hitOld).length();
    Quat quat(axis,angle);


    getSceneGraph()->getCamera()->rotation_matrix() *= quat.convertToMatrix();

    mDistance = (getSceneGraph()->getCamera()->position() - getSceneGraph()->getCamera()->center()).length();
    this->updateCamera();
}


void NormalManipulator::pan(int posX, int posY)
{
    // do nothing
}

void NormalManipulator::zoom(const float& delta)
{
    mDistance = (getSceneGraph()->getCamera()->position() - getSceneGraph()->getCamera()->center()).length();
    mDistance *= (1 + delta);
    this->updateCamera();
}

}}


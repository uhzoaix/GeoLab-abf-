#include "TrackBallManipulator.h"
#include "../SceneGraph.h"
#include "../Camera.h"
#include "../Decorations/Arcball.h"
namespace GCL { namespace SceneGraph {
TrackBallManipulator::TrackBallManipulator(SceneGraphNode* _scenegraph)
    :NormalManipulator(_scenegraph)
{
    mDefaultCenter = getSceneGraph()->getCamera()->center();
    mDefaultDistance = mDistance = (getSceneGraph()->getCamera()->position() - getSceneGraph()->getCamera()->center()).length();
 }

void TrackBallManipulator::rotate(int posX, int posY)
{
    Vec3 hitOld;
    int hit0 = hitSphere(mCurrentX,mCurrentY, hitOld);
    Vec3 hitNew;
    int hit1 = hitSphere(posX,posY,hitNew);
    if(hit0 != hit1) return;
//    if((hitNew - hitOld).length() / this->getRadius() > M_PI / 7) return;
    Vec3 center = getSceneGraph()->getCamera()->center();
    Vec3 axis = (hitOld - center) ^ (hitNew - center);
    if(axis.Normalize() < ZERO_TOLERANCE) return;
    Scalar angle = (hitNew - hitOld).length() / this->getRadius();
    Quat quat(axis,angle);
    getSceneGraph()->getCamera()->rotation_matrix() *= quat.convertToMatrix();
}

void TrackBallManipulator::pan(int posX, int posY)
{
    Vec3 hitOld;
    int hit0 = hitCenterPlane(mCurrentX,mCurrentY, hitOld);
    Vec3 hitNew;
    int hit1 = hitCenterPlane(posX,posY,hitNew);
    if(hit0 && hit1)
    {
        getSceneGraph()->getCamera()->move(hitNew - hitOld);
        arcball_->setOffset(getSceneGraph()->getCamera()->getOffset());
    }

}


Scalar TrackBallManipulator::getRadius() const
{
    return this->mDistance / 3.0;
}

bool TrackBallManipulator::getIntersectionToSphere(const Vec3 &rayPoint, const Vec3 &rayDir, Vec3 &intersect, bool isNearOne)
{
    Scalar r = getRadius();
    Vec3 center = getSceneGraph()->getCamera()->center();
    Scalar b = Vec3::getDistanceFromRayToPoint(rayPoint,rayDir,center);
    Scalar c = r * r - b * b;
    if(c < 0) return false;
    c = sqrt(c);
    Vec3 closestPoint = Vec3::getClosestPointFromPointToRay(center,rayPoint,rayDir);

    intersect = closestPoint + rayDir * c;
    if(isNearOne)
    {
        if((closestPoint - rayDir * c - rayPoint).length() < (intersect - rayPoint).length())
        {
            intersect = closestPoint - rayDir * c;
        }
    }
    else
    {
        if((closestPoint - rayDir * c - rayPoint).length() > (intersect - rayPoint).length())
        {
            intersect = closestPoint - rayDir * c;
        }
    }

    return true;
}




int TrackBallManipulator::hitSphere(int posX, int posY, Vec3 &intersect)
{
    Vec3 rayPoint;
    Vec3 rayDir;
    getSceneGraph()->getCamera()->get_project_ray(posX,posY,rayPoint,rayDir);
    rayDir.Normalize();
    Vec3 center = getSceneGraph()->getCamera()->center();
    Quat quat;
    quat.convertFromMatrix(getSceneGraph()->getCamera()->get_model_matrix());
    HomoMatrix4 inverseModelMatrix = quat.conjugate().convertToMatrix();
    Vec3 np = inverseModelMatrix * Vec3(0,0,1);
    Vec3 hitPlane;
    if(Vec3::getIntersectionRayToPlane(rayPoint,rayDir,center,np,hitPlane))
    {
        Vec3 v = hitPlane - center;
        Scalar len = v.length();
        bool isNearOne = true;
        v.Normalize();
        int k = len / this->getRadius();

        Scalar residual = len - this->getRadius() * k;
        Vec3 planePoint = hitPlane;
        if(k % 4== 0)
        {
            planePoint = center + v * residual;
        }
        else if(k % 4== 1)
        {
            planePoint = center + v * (this->getRadius() - residual);
            isNearOne = false;
        }
        else if(k % 4== 2)
        {
            planePoint = center - v * residual;
            isNearOne = false;
        }
        else
        {
            planePoint = center - v * (this->getRadius() - residual);
        }
        Vec3 dir = planePoint - rayPoint;
        dir.Normalize();
        if(getIntersectionToSphere(rayPoint,dir,intersect,isNearOne))
        {
            return k % 4;
        }
    }
    intersect = Vec3::getClosestPointFromPointToRay(center,rayPoint,rayDir);
    return -1;
}

int TrackBallManipulator::hitCenterPlane(int posX, int posY, Vec3 &intersect)
{

    Vec3 center = getSceneGraph()->getCamera()->center();
    Vec3 np = getSceneGraph()->getCamera()->get_direction();
    Vec3 rayPoint,rayDir;
    getSceneGraph()->getCamera()->get_project_ray(posX,posY,rayPoint,rayDir);
    rayDir.Normalize();
    return Vec3::getIntersectionRayToPlane(rayPoint,rayDir,center,np,intersect);
}
}}

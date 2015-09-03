#ifndef TRACKBALLMANIPULATOR_H
#define TRACKBALLMANIPULATOR_H
/*
Authors: Shiwei Wang, Kang Wang
Email(s): wangshiwei@abaci3d.com

*/
#include "gcl_global.h"
#include "NormalManipulator.h"
#include "../../Math/MathDefines.h"
namespace GCL { namespace SceneGraph {

class GCL_CORE_SHARED_EXPORT TrackBallManipulator : public NormalManipulator
{
public:
    TrackBallManipulator(SceneGraphNode* _scenegraph);
    const char* getEventHandlerName() const {return "TrackManipulator";}
    virtual ~TrackBallManipulator() {}

protected:


    virtual void rotate(int posX, int posY);
    virtual void pan(int posX, int posY);
private:
    Scalar getRadius() const;

    bool getIntersectionToSphere(const Vec3& rayPoint, const Vec3& rayDir,Vec3& intersect,bool isNearOne = true);
    int hitSphere(int posX, int posY, Vec3& intersect);
    int hitCenterPlane(int posX, int posY, Vec3& intersect);



};
}}

#endif // TRACKBALLMANIPULATOR_H

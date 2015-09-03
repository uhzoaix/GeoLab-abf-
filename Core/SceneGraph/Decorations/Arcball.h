#ifndef GCL_RENDERELEMENTS_ARCBALL_H
#define GCL_RENDERELEMENTS_ARCBALL_H
#include "gcl_global.h"
#include "DecorationBase.h"
#include "Core/Math/MathDefines.h"
namespace GCL { namespace Decoration {

/**
    Acrball:  showing a arc ball  for manipulator

    \author Shiwei Wang
**/
class GCL_CORE_SHARED_EXPORT Arcball : public DecorationBase
{
public:
    Arcball();
    ~Arcball() {}

    void render();
    void setOffset(const Vec3& offset) {offset_ = offset;}
    void setRadius(float r) {radius_ = r;}
private:

    Vec3 offset_;
    float radius_;
    int width_;
    int height_;

};
}}
#endif // ARCBALL_H

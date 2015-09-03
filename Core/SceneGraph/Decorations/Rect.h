#ifndef GCL_RENDERELEMETS_RECT_H
#define GCL_RENDERELEMETS_RECT_H
#include "gcl_global.h"
#include "DecorationBase.h"
#include "Core/Math/MathDefines.h"

namespace GCL { namespace Decoration {

class GCL_CORE_SHARED_EXPORT Rect : public DecorationBase
{
public:

    Rect();
    virtual ~Rect() {}
    virtual void render();

    void setStartPoint(float x, float y) {v_[0] = x; v_[1] = y;v_[2] = x; v_[3] = y;}
    void setEndPoint(float x, float y) {v_[2] = x; v_[3] = y;}
    void setRect(const Vec4& v) {v_ = v;}
private:
    Vec4 v_;


};
}}
#endif // RECT_H

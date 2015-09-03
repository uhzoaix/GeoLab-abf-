#ifndef GCL_GLSTATES_H
#define GCL_GLSTATES_H
#include "gcl_global.h"
#include "Math/MathDefines.h"
namespace GCL { namespace SceneGraph {

class GCL_CORE_SHARED_EXPORT GLStates
{
public:
    explicit GLStates();
    ~GLStates();


    void updateGL();



    bool lighting() const;
    void setLighting(bool lighting);

    bool depth_test() const;
    void setDepth_test(bool depth_test);

    bool color_material() const;
    void setColor_material(bool color_material);

    bool blend() const;
    void setBlend(bool blend);



    Vec4 background_color_top() const;
    void setBackground_color_top(const Vec4 &background_color_top);

    Vec4 background_color_bottom() const;
    void setBackground_color_bottom(const Vec4 &background_color_bottom);

private:
    bool lighting_;
    bool depth_test_;
    bool color_material_;
    bool blend_;


    Vec4 background_color_top_;
    Vec4 background_color_bottom_;

};
}}
#endif // GLSTATES_H

#ifndef GCL_SCENEGRAPH_CAMERA_H
#define GCL_SCENEGRAPH_CAMERA_H
#include "gcl_global.h"
#include "../Math/MathDefines.h"
namespace GCL{ namespace SceneGraph {
/**


*/
class GCL_CORE_SHARED_EXPORT Camera
{
public:
    Camera();
    void enter();
    void leave();
    const Vec3& position() const {return pos_;}
    Vec3& position() {return pos_;}
    const Vec3& center() const {return center_;}
    Vec3& center() {return center_;}
    const Vec3& up() const {return up_;}
    Vec3& up() {return up_;}
    void set_position(const Vec3& pos) {pos_ = pos;}
    void set_center(const Vec3 &center) {center_ = center;}
    void set_up(const Vec3 &up) {up_ = up;}
    void set_position(Scalar x, Scalar y, Scalar z) {pos_[0] = x; pos_[1] = y; pos_[2] = z;}
    void set_center(Scalar x, Scalar y, Scalar z) {center_[0] = x; center_[1] = y; center_[2] = z;}
    void sut_up(Scalar x, Scalar y, Scalar z ) {up_[0] = x; up_[1] = y; up_[2] = z; }

    const HomoMatrix4& rotation_matrix() const {return rotate_matrix_;}
    HomoMatrix4& rotation_matrix() {return rotate_matrix_;}



    const HomoMatrix4& get_project_matrix() const { return project_matrix_;}
    const HomoMatrix4& get_model_matrix() const { return model_matrix_; }
    const Vec4i& get_viewport() const {return viewport_;}
    void set_viewport(int w, int h);
    void get_project_ray(int posx, int posy, Vec3& ray_start, Vec3& ray_direction) const;
    Vec3 get_camera_position() const;
    Vec3 getCameraPosition(int posx, int posy) const;
    Vec3 get_direction() const;
    void move(const Vec3& v) {offset_ += v;}
    Vec3 getOffset() const {return offset_;}
    void set_default_offset() {offset_ = Vec3();}
    Scalar distance() const;
private:
    void update_viewport_and_matrix();
    Vec3 pos_;
    Vec3 center_;
    Vec3 up_;

    Vec4i viewport_;
    HomoMatrix4 project_matrix_;
    HomoMatrix4 model_matrix_;

    HomoMatrix4 rotate_matrix_;
    Vec3 offset_;
    int width_;
    int height_;

};
}}

#endif // CAMERA_H

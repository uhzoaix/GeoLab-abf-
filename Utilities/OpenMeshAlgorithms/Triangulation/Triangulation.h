#ifndef GCL_TRIANGULATION_H
#define GCL_TRIANGULATION_H
#include <Utilities/OpenMeshAlgorithms/gcl_export.h>
#include <QString>
#include <QList>
#include <Core/Math/MathDefines.h>
namespace GCL { namespace Utilities {
/**
 * @brief The Triangulation class
 *      using triangle.c to triangulating
 *
 */
class GCL_OPENMESHALGORITHMS_SHARED_EXPORT Triangulation
{
public:
    Triangulation();
    ~Triangulation();


    void apply(
            const QList<Vec3>& input_vertices_positions,
            const QList<Vec2i >& input_edges,
            const QList<int>& input_boundary_markers,
            QList<Vec3>& output_vertices_positions,
            QList<Vec3i>& output_triangles
            );




    /**
     * @brief set_quality_minimum_angle
     * @param angle  if used (0-30°),  else -1
     */
    void set_quality_minimum_angle(double angle) {quality_minimum_angle_ = angle;}
    double get_quality_minimum_angle() const {return quality_minimum_angle_;}


    /**
     * @brief set_quality_maximum_aree
     * @param area  if used ( > 0),  else -1
     */
    void set_quality_maximum_area(double area) {quality_maximun_area_ = area;}
    double get_quality_maximum_area() const {return quality_maximun_area_;}


    void set_enclose_the_convex_hull(bool t) {enclose_the_convex_hull_ = t;}
    bool is_enclosing_the_convex_hull() const {return enclose_the_convex_hull_;}

    void set_enable_boundary_splitting(bool t) {suppress_boundary_splitting_ = !t;}
    bool enable_boundary_splitting() const {return suppress_boundary_splitting_;}

    void set_conforming_delaunay(bool t) {conformming_delaunay_ = t;}
    bool is_conforming_delaunay() const {return conformming_delaunay_;}

    void set_maximum_of_added_steiner_points(int maxi) {maximum_of_added_steiner_points_ = maxi;}
    int get_maximum_of_added_steiner_points() const {return maximum_of_added_steiner_points_;}


    void set_quiet(bool t) {quiet_ = t;}
    bool is_quiet() const {return quiet_;}
private:
    bool init_plane(const QList<Vec3>& input_vertices_positions);
    Vec3 project_to_plane(const Vec3& v);
    Vec3 unproject_to_plane(const Vec3& v);


    QString get_triangle_command();


    double quality_minimum_angle_; ///* if -1, not use; if > 0, use it;
    double quality_maximun_area_; ///* if -1, not use; if  > 0, use it;
    bool enclose_the_convex_hull_;
    bool conformming_delaunay_;
    int  maximum_of_added_steiner_points_; ///* if -1, not use; if > 0, use it;
    bool suppress_boundary_splitting_;

    bool quiet_;

    Vec3 plane_normal_;
    Vec3 plane_center_;
    Vec3 plane_x_axis_;
    Vec3 plane_y_axis_;

};
}}
#endif // TRIANGULATION_H

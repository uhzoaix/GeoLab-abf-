#ifndef CYLINDERCLIPPING_H
#define CYLINDERCLIPPING_H
#include <Utilities/OpenMeshAlgorithms/gcl_export.h>
#include <GCL/Core/Math/MathDefines.h>
#include <vector>
namespace GCL {
class OMesh;


namespace Utilities {

/**
 * @brief The CylinderClipping class
 * @author Shiwei Wang
 * @date 2015.3.10
 *
 */


class GCL_OPENMESHALGORITHMS_SHARED_EXPORT CylinderClipping
{
public:

    struct GCL_OPENMESHALGORITHMS_SHARED_EXPORT Cylinder
    {

        Cylinder() : radius_(1),slices_(3) {}

        bool generatePoints(const Vec3& start_point);
        Vec3 cyl_center_;
        Vec3 cyl_direction_;
        Scalar radius_;
        int slices_;

        std::vector<Vec3> cyl_points_;
    };

    CylinderClipping() {}



    bool apply(OMesh* mesh,  const Vec3& cyl_center, const Vec3& cyl_dirction, Scalar radius,
                      int slices);


private:
    bool is_point_inside_cylinder(const Vec3& point, const Cylinder& cyl);

    int intersection_line_cylinder(const Vec3& line_v0, const Vec3& line_v1,
                                           const Cylinder& cyl, std::vector<Vec3>& intersections);



};
}}
#endif // CYLINDERCLIPPING_H

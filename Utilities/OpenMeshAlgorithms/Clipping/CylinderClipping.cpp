#include "CylinderClipping.h"
#include <GCL/Core/DataStructures/OMesh.h>
namespace GCL { namespace Utilities {

bool CylinderClipping::apply(OMesh *mesh, const Vec3 &cyl_center, const Vec3 &cyl_dirction, Scalar radius, int slice)
{
    Cylinder cyl;
    cyl.cyl_center_ = cyl_center;
    cyl.cyl_direction_ = cyl_dirction;
    cyl.radius_ = radius;
    cyl.slices_ = slice;
    cyl.generatePoints(Vec3(0,1,0));

    for(size_t i=0; i < mesh->getFacesNumber(); i++)
    {
        OMesh::FaceHandle fh = mesh->face_handle_(i);
        OMesh::TriIndices tri = mesh->getFaceTriIndices(i);
        for(int j=0; j < 3; j++)
        {
            Vec3 point = mesh->getVertexPosition(tri[j]);
            if(is_point_inside_cylinder(point,cyl))
            {
                mesh->status_(fh).set_selected(true);
            }
        }

    }
    DebugLog<<"Finish"<<DebugEnd;

    return true;
}

bool CylinderClipping::is_point_inside_cylinder(const Vec3 &point, const CylinderClipping::Cylinder &cyl)
{
    double d = Vec3::getDistanceFromRayToPoint(cyl.cyl_center_,cyl.cyl_direction_,point);
    DebugLog<<cyl.radius_<<std::endl;
    if(d > cyl.radius_) return false;

    return true;
    for(size_t i=0; i < cyl.cyl_points_.size(); i++)
    {
        Vec3 v0  = cyl.cyl_points_[i];
        Vec3 v1  = cyl.cyl_points_[(i+1) % cyl.cyl_points_.size()];

        Vec3 vc = (v0 + v1) * 0.5;
        Vec3 vd = vc - cyl.cyl_center_;
        vd.Normalize();
        if((point - cyl.cyl_center_) * vd < -TOLERANCE)
        {
            return false;
        }

    }

    return true;

}

int CylinderClipping::intersection_line_cylinder(const Vec3 &line_v0, const Vec3 &line_v1, const CylinderClipping::Cylinder &cyl, std::vector<Vec3> &intersections)
{
    bool inside_v0 = is_point_inside_cylinder(line_v0,cyl);
    bool inside_v1 = is_point_inside_cylinder(line_v1,cyl);
    if(inside_v0 && inside_v1)
    {
        return 1;
    }
    int flag = 0;
    for(size_t i=0; i < cyl.cyl_points_.size(); i++)
    {
        Vec3 v0  = cyl.cyl_points_[i];
        Vec3 v1  = cyl.cyl_points_[(i+1) % cyl.cyl_points_.size()];
        Vec3 intersection;
        if(Vec3::checkSegmentsIntersection(v0,v1,line_v0,line_v1,intersection))
        {
            intersections.push_back(intersection);
            flag = 1;
        }

    }
    return flag;
}

bool CylinderClipping::Cylinder::generatePoints(const Vec3 &start_point)
{
    Scalar step_angle = 2.0 * M_PI / (Scalar)slices_;

    Vec3 v = Vec3::getClosestPointFromPointToRay(start_point,cyl_center_,cyl_direction_);
    if( (v - start_point).length() < TOLERANCE)
    {
        return false;
    }
    Vec3 v0 = v;
    Vec3 vd = start_point - v0;
    vd.Normalize();
    Vec3 vx = vd * radius_;
    Vec3 vy = vd ^ cyl_direction_;
    vy.Normalize();
    vy = vy * radius_;
    cyl_points_.clear();
    for(int i=0; i < slices_; i++)
    {
        Scalar angle = step_angle * i;
        Vec3 pos = cyl_center_ + vx * cos(angle) + vy * sin(angle);
        cyl_points_.push_back(pos);
    }
    return true;

}



}}


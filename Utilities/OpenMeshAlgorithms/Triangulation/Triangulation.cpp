#include "Triangulation.h"
#define NO_TIMER
#include <cstdlib>
#include <sstream>
#include "triangle/triangle.c"
#include "triangle/triangle.h"

namespace GCL  { namespace Utilities  {


Triangulation::Triangulation()
    :quality_minimum_angle_(-1),quality_maximun_area_(-1),enclose_the_convex_hull_(false),
    conformming_delaunay_(false),maximum_of_added_steiner_points_(-1),suppress_boundary_splitting_(true),
    quiet_(false)
{

}

Triangulation::~Triangulation()
{

}



void initTriangulateio(triangulateio& input)
{
    input.pointlist = NULL;
    input.pointattributelist = NULL;
    input.pointmarkerlist = NULL;
    input.trianglelist = NULL;
    input.triangleattributelist = NULL;
    input.trianglearealist = NULL;
    input.neighborlist = NULL;
    input.segmentlist = NULL;
    input.segmentmarkerlist = NULL;
    input.holelist = NULL;
    input.regionlist = NULL;
    input.edgelist = NULL;
    input.edgemarkerlist = NULL;
    input.normlist = NULL;
    input.numberofpoints = 0;
    input.numberofpointattributes = 0;
    input.numberoftriangles = 0;
    input.numberofcorners = 0;
    input.numberoftriangleattributes = 0;
    input.numberofsegments = 0;
    input.numberofholes = 0;
    input.numberofregions = 0;
    input.numberofedges = 0;
}

void freeTriangualteio(triangulateio& input)
{
    if(input.pointlist)
    {
        free(input.pointlist);
        input.pointlist = NULL;
    }
    if(input.pointattributelist)
    {
        free(input.pointattributelist);
        input.pointattributelist = NULL;
    }
    if(input.pointmarkerlist)
    {
        free(input.pointmarkerlist);
        input.pointmarkerlist = NULL;
    }
    if(input.segmentlist)
    {
        free(input.segmentlist);
        input.segmentlist = NULL;
    }
    if(input.segmentmarkerlist)
    {
        free(input.segmentmarkerlist);
        input.segmentmarkerlist = NULL;
    }
    if(input.regionlist)
    {
        free(input.regionlist);
        input.regionlist = NULL;
    }
    if(input.holelist)
    {
        free(input.holelist);
        input.holelist = NULL;
    }
    if(input.neighborlist)
    {
        free(input.neighborlist);
        input.neighborlist = NULL;
    }
    if(input.edgelist)
    {
        free(input.edgelist);
        input.edgelist = NULL;
    }
    if(input.edgemarkerlist)
    {
        free(input.edgemarkerlist);
        input.edgemarkerlist = NULL;
    }
    if(input.trianglearealist)
    {
        free(input.trianglearealist);
        input.trianglearealist = NULL;
    }
    if(input.triangleattributelist)
    {
        free(input.triangleattributelist);
        input.triangleattributelist = NULL;
    }
    if(input.trianglelist)
    {
        free(input.trianglelist);
        input.trianglelist = NULL;
    }
    if(input.normlist)
    {
        free(input.normlist);
        input.normlist = NULL;
    }


}

void Triangulation::apply(const QList<Vec3> &input_vertices_positions, const QList<Vec2i> &input_edges, const QList<int> &input_boundary_markers, QList<Vec3> &output_vertices_positions, QList<Vec3i> &output_triangles)
{
   //0: init plane
   this->init_plane(input_vertices_positions);

   //1: init
   triangulateio input, output;
   initTriangulateio(input);
   initTriangulateio(output);

   input.numberofpoints = input_vertices_positions.size();
   input.numberofsegments = input_edges.size();
   input.pointlist = (REAL *) malloc( ( input.numberofpoints ) * 2 * sizeof(REAL));
   input.segmentlist = (int *)malloc( input.numberofsegments * 2 * sizeof( int ) );
   for (int i=0;i< input.numberofpoints;i++)
   {
       Vec3 v = project_to_plane(input_vertices_positions[i]);
       input.pointlist[2 * i] = v[0];
       input.pointlist[2 * i + 1] = v[1];
   }
   for (int i=0;i< input_edges.size();i++)
   {
       input.segmentlist[2 * i] = input_edges[i][0];
       input.segmentlist[2 * i + 1] = input_edges[i][1];
   }
   if(input_edges.size() == input_boundary_markers.size())
   {
       input.segmentmarkerlist = (int *)malloc( input.numberofsegments * sizeof(int) );
       for(int i=0; i < input_boundary_markers.size(); i++)
       {
           input.segmentmarkerlist[i] = input_boundary_markers[i];
       }
   }

   //2: apply

   QString cmd = this->get_triangle_command();

   char* t_cmd = strdup(cmd.toStdString().c_str());
   triangulate(t_cmd,&input, &output,(struct triangulateio *) NULL);

   //3: output
   output_triangles.clear();
   output_vertices_positions.clear();

   for(int i=0; i < output.numberofpoints; i++)
   {
       output_vertices_positions.push_back(unproject_to_plane( Vec3(output.pointlist[2 * i],output.pointlist[2 * i + 1],0)));
   }
   for(int i=0; i < output.numberoftriangles; i++)
   {
       output_triangles.push_back(Vec3i(output.trianglelist[3*i],output.trianglelist[3*i+1],output.trianglelist[3*i+2]));
   }

   //4: free
   free(t_cmd);
   freeTriangualteio(input);
   freeTriangualteio(output);
}

bool Triangulation::init_plane(const QList<Vec3> &input_vertices_positions)
{
    if(input_vertices_positions.size() < 2) return false;
    plane_center_ = input_vertices_positions[0];
    plane_x_axis_ = input_vertices_positions[1] - input_vertices_positions[0];
    plane_x_axis_.Normalize();
    bool flag = false;
    for(int i=3; i< input_vertices_positions.size(); i++)
    {
        Vec3 v = input_vertices_positions[i] - plane_center_;
        v.Normalize();
        Vec3 normal = v ^ plane_x_axis_;
        if(normal.Normalize() > TOLERANCE)
        {
            plane_normal_ = normal;
            plane_y_axis_ = plane_x_axis_ ^ plane_normal_;
            plane_y_axis_.Normalize();
            flag  = true;
            break;
        }

    }

    return flag;



}

Vec3 Triangulation::project_to_plane(const Vec3 &v)
{
    Vec3 vout;
    vout[0] = (v - plane_center_) * plane_x_axis_;
    vout[1] = (v - plane_center_) * plane_y_axis_;
    return vout;
}

Vec3 Triangulation::unproject_to_plane(const Vec3 &v)
{
    Vec3 vout;
    vout = plane_center_ + plane_x_axis_ * v[0] + plane_y_axis_ * v[1];
    return vout;
}

QString Triangulation::get_triangle_command()
{
    QString cmd = "pz";
    if(suppress_boundary_splitting_)
    {
       cmd += "Y";
    }
    if(quality_minimum_angle_ > 0)
    {
        cmd += "q"+QString::number(quality_minimum_angle_);
    }
    if(quality_maximun_area_ > 0)
    {
        cmd += "a"+QString::number(quality_maximun_area_);
    }
    if(enclose_the_convex_hull_)
    {
        cmd += "c";
    }
    if(conformming_delaunay_)
    {
        cmd += "D";
    }
   if(maximum_of_added_steiner_points_ > 0)
   {
       cmd += "S"+QString::number(maximum_of_added_steiner_points_);
   }
   if(quiet_)
   {
       cmd += "Q";
   }
   return cmd;
}

}}

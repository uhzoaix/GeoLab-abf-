#include "MeshIntersections.h"
#include <Utilities/OpenMeshAlgorithms/Geometry/NearestTriangleSearch/NearestTrianglesQuery.h>
#include <memory>
#include <ctime>
#include <set>
namespace GCL { namespace Utilities {


MeshIntersections::MeshIntersections()
{
    mesh_[0] = NULL; mesh_[1] = NULL;
}

MeshIntersections::~MeshIntersections()
{

}

void MeshIntersections::compute_intersections(OMesh *mesh0, OMesh *mesh1)
{
    this->init(mesh0,mesh1);
    this->find_intersections();
}

void MeshIntersections::init(OMesh *mesh0, OMesh *mesh1)
{
    mesh_[0] = mesh0;
    mesh_[1] = mesh1;

    vertices_.clear();
    std::vector<Vertex>(vertices_).swap(vertices_);
    segments_.clear();
    std::vector<Segment>(segments_).swap(segments_);
    vert_map.clear();
    std::map<Vec3,int>(vert_map).swap(vert_map);
}

void MeshIntersections::find_intersections()
{
    /// 1: built tree
    GCL::Utilities::NearestTrianglesFinder finder;
    finder.build_tree(mesh_[1]);
    std::cout<<"build tree"<<" "<<mesh_[0]->getFacesNumber()<<" "<<mesh_[1]->getFacesNumber()<<std::endl;
    clock_t t0 = clock();
    /// 2: find intersetions on tree
    for(size_t i=0; i < mesh_[0]->getFacesNumber(); i++)
    {
        //        std::cout<<i<<std::endl;
        GCL::OMesh::FaceHandle fh_on_mesh0 = mesh_[0]->face_handle_(i);
        GCL::OMesh::TriIndices indices = mesh_[0]->getFaceTriIndices(i);
        GCL::Vec3 v[3];
        for(int j=0; j < 3; j++)
        {
            v[j] = mesh_[0]->getVertexPosition(indices[j]);
        }
        std::shared_ptr<GCL::Utilities::TriangleIntersectionQuery> query(new GCL::Utilities::TriangleIntersectionQuery(v[0],v[1],v[2]));
        finder.query_on_tree(query.get());
        if(!query->flag()) continue;
        for(size_t j=0; j < query->get_facehandles().size(); j++)
        {
            OMesh::FaceHandle fh_on_mesh1 = query->get_facehandles().at(j);
            this->compute_intersections(fh_on_mesh0,fh_on_mesh1);
        }

    }
    std::cout<<clock() - t0<<std::endl;
    this->check_circle();
}

bool MeshIntersections::compute_intersections(OMesh::FaceHandle fh_on_mesh0, OMesh::FaceHandle fh_on_mesh1)
{
    Vec3 intersection_on_face_0[2];
    Vec3 intersection_on_face_1[2];
    Vec3 face_0_vertices[3];
    Vec3 face_1_vertices[3];
    int index = 0;
    for(OMesh::FaceVertexIter itr=mesh_[0]->fv_iter_(fh_on_mesh0); itr.is_valid(); itr++)
    {
        face_0_vertices[index++] = mesh_[0]->point_(*itr);
    }
    index = 0;
    for(OMesh::FaceVertexIter itr=mesh_[1]->fv_iter_(fh_on_mesh1); itr.is_valid(); itr++)
    {
        face_1_vertices[index++] = mesh_[1]->point_(*itr);
    }

    Vec3 face_0_normal = (face_0_vertices[1] - face_0_vertices[0]) ^ (face_0_vertices[2] - face_0_vertices[0]);
    face_0_normal.Normalize();
    Vec3 face_1_normal = (face_1_vertices[1] - face_1_vertices[0]) ^ (face_1_vertices[2] - face_1_vertices[0]);
    face_1_normal.Normalize();


    int intersection_on_face_0_count = 0;
    for(int i=0; i < 3; i++)
    {
        Vec3 v1 = face_0_vertices[(i+1) % 3];
        Vec3 v0 = face_0_vertices[i];
        Vec3 intersect;
        Vec3 vdir = (v1 - v0);
        vdir.Normalize();
        if(!Vec3::getIntersectionRayToPlane(v0,vdir,face_1_vertices[0],face_1_normal,intersect)) continue;
        Scalar dist0 = (v1 - v0) * vdir;
        Scalar dist1 = (intersect - v0) * vdir;
        if(dist1 > -TOLERANCE && dist1 < dist0 + TOLERANCE)
        {
            intersection_on_face_0[intersection_on_face_0_count++] = intersect;
            if(intersection_on_face_0_count >= 2) break;
        }
    }

    int intersection_on_face_1_count = 0;
    for(int i=0; i < 3; i++)
    {
        Vec3 v1 = face_1_vertices[(i+1) % 3];
        Vec3 v0 = face_1_vertices[i];
        Vec3 intersect;
        Vec3 vdir = (v1 - v0);
        vdir.Normalize();
        if(!Vec3::getIntersectionRayToPlane(v0,vdir,face_0_vertices[0],face_0_normal,intersect)) continue;
        Scalar dist0 = (v1 - v0) * vdir;
        Scalar dist1 = (intersect - v0) * vdir;
        if(dist1 > -TOLERANCE && dist1 < dist0 + TOLERANCE)
        {
            intersection_on_face_1[intersection_on_face_1_count++] = intersect;
            if(intersection_on_face_1_count >= 2) break;
        }
    }

    Vec3 vdir0 = intersection_on_face_0[1] - intersection_on_face_0[0];
    //    Vec3 vdir1 = intersection_on_face_1[1] - intersection_on_face_1[0];
    vdir0.Normalize();
    Scalar d0 = 0.0;
    Scalar d1 = (intersection_on_face_0[1] - intersection_on_face_0[0]) * vdir0;
    Scalar d2 = (intersection_on_face_1[0] - intersection_on_face_0[0]) * vdir0;
    Scalar d3 = (intersection_on_face_1[1] - intersection_on_face_0[0]) * vdir0;
    if(d2 > d3)
    {
        Scalar tmp = d2; d2 = d3; d3 = tmp;
    }
    if(d3 < -TOLERANCE || d2 > d1 + TOLERANCE)
    {
        return false;
    }
    Scalar ans_d0 = (d0>d2)?d0:d2;
    Scalar ans_d1 = (d1<d3)?d1:d3;
    Vec3 seg_v0 = intersection_on_face_0[0] + vdir0 * ans_d0;
    Vec3 seg_v1 = intersection_on_face_0[0] + vdir0 * ans_d1;
    this->add_new_segment(seg_v0,seg_v1,fh_on_mesh0,fh_on_mesh1);


    return true;
}

void MeshIntersections::add_new_segment(const Vec3 &v0, const Vec3 &v1, OMesh::FaceHandle fh_on_mesh0, OMesh::FaceHandle fh_on_mesh1)
{
    int id0 = get_vertex_id(v0);
    int id1 = get_vertex_id(v1);
    Segment segment;
    segment.vid_[0] = id0;
    segment.vid_[1] = id1;
    segment.on_face_[0] = fh_on_mesh0.idx();
    segment.on_face_[1] = fh_on_mesh1.idx();
    segments_.push_back(segment);
    if(vertices_[id0].sid_[0] == -1)
    {
        vertices_[id0].sid_[0] = segments_.size() - 1;
    }
    else if(vertices_[id0].sid_[1] == -1)
    {
        vertices_[id0].sid_[1] = segments_.size() - 1;
    }
    else
    {
        std::cout<<"Wrong"<<std::endl;
    }

    if(vertices_[id1].sid_[0] == -1)
    {
        vertices_[id1].sid_[0] = segments_.size() - 1;
    }
    else if(vertices_[id1].sid_[1] == -1)
    {
        vertices_[id1].sid_[1] = segments_.size() - 1;
    }
    else
    {
        std::cout<<"Wrong"<<std::endl;
    }


}

int MeshIntersections::get_vertex_id(const Vec3 &v)
{
    if(vert_map.count(v))
    {
        return  vert_map.at(v);
    }
    else
    {
        Vertex vx;
        vx.pos_ = v;
        vx.vid_ = vertices_.size();
        vertices_.push_back(vx);
        vert_map[v] = vx.vid_;
        return vx.vid_;
    }
}

bool MeshIntersections::check_circle()
{
    std::set<int> visited_segment;
    std::cout<<vertices().size()<<" "<<segments_.size()<<std::endl;

    for(int i=0; i < (int)segments_.size(); i++)
    {
        if(visited_segment.count(i)) continue;
        int sid = i;
        int vid = segments_[i].vid_[0];
        std::cout<<std::endl<<sid<<" ";
        while(visited_segment.count(sid) == 0)
        {
            visited_segment.insert(sid);
            Vertex vertex = vertices_[vid];
            if(vertex.sid_[0] == -1 || vertex.sid_[1] == -1)
            {
                std::cout<<"break: type 1"<<std::endl;
                break;
            }

            if(vertex.sid_[0] == sid)
            {
                sid = vertex.sid_[1];
            }
            else
            {
                sid = vertex.sid_[0];
            }
            Segment segment = segments_[sid];
            if(segment.vid_[0] == -1 || segment.vid_[1] == -1)
            {
                std::cout<<"break: type 2"<<std::endl;
                break;
            }
            if(segment.vid_[0] == vid)
            {
                vid = segment.vid_[1];
            }
            else
            {
                vid = segment.vid_[0];
            }
            std::cout<<sid<<" ";

        }



    }

    return true;
}

}}

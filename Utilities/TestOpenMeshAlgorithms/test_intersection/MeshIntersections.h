#ifndef MESHINTERSECTIONS_H
#define MESHINTERSECTIONS_H
#include <GCL/Core/DataStructures/OMesh.h>
#include <vector>
#include <map>
namespace GCL { namespace Utilities{
class MeshIntersections
{
public:
    struct Vertex
    {
        Vertex(): vid_(-1)
        {
            sid_[0] = sid_[1] = -1;
        }

        int vid_;
        int sid_[2];
        Vec3 pos_;
    };

    struct Segment
    {
        int vid_[2];
        int on_face_[2];
    };

    MeshIntersections();
    ~MeshIntersections();

    void compute_intersections(OMesh* mesh0, OMesh* mesh1);



    std::vector< Vertex >& vertices() {return vertices_;}
    const std::vector< Vertex >& vertices() const {return vertices_;}
    std::vector< Segment >& segments() {return segments_;}
    const std::vector< Segment >& segments() const {return segments_;}
private:
    void init(OMesh* mesh0, OMesh* mesh1);
    void find_intersections();

    bool compute_intersections(
            OMesh::FaceHandle fh_on_mesh0,
            OMesh::FaceHandle fh_on_mesh1
            );

    void add_new_segment(const Vec3& v0, const Vec3& v1,  OMesh::FaceHandle fh_on_mesh0,OMesh::FaceHandle fh_on_mesh1);
    int get_vertex_id(const Vec3& v);

    bool check_circle();
    OMesh* mesh_[2];
    std::vector< Vertex > vertices_;
    std::vector< Segment > segments_;
    std::map<Vec3,int> vert_map;
};
}}
#endif // MESHINTERSECTIONS_H

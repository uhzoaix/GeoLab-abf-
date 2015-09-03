#ifndef GCL_CORE_LIGHTMESH_H
#define GCL_CORE_LIGHTMESH_H
#include "GeometryBase.h"
#include "Core/gcl_global.h"
#include <vector>
namespace GCL {
class GCL_CORE_SHARED_EXPORT LightMesh : public GeometryBase
{
public:
    LightMesh();
    virtual ~LightMesh();

    struct Vertex
    {
        Vertex():is_selected_(false),id_(-1){}
        void add_neighbor(int nid)
        {
            for(size_t i=0; i < neighbors_.size(); i++)
            {
                if(neighbors_[i] == nid) return;
            }
            neighbors_.push_back(nid);
        }
        void add_face(int fid)
        {
            for(size_t i=0; i < face_ids_.size(); i++)
            {
                if(face_ids_[i] == fid) return;
            }
            face_ids_.push_back(fid);
        }

        int id_;
        Vec3 pos_;
        Vec3 normal_;
        Vec4 color_;
        bool is_selected_;
        std::vector<int> face_ids_;
        std::vector<int> neighbors_;
    };
    struct Face
    {
        Face():is_selected_(false),id_(-1){}
        int id_;
        Vec3i indices_;
        Vec3 normal_;
        Vec4 color_;
        bool is_selected_;

    };
public: /// create mesh
    void clear();
    int add_point(const Vec3& pos);
    int add_face(const Vec3i& indices);




    void update_normals();


    int get_number_of_vertex_face(int vid) const;
    int get_face_id_of_vertex(int vid, int index) const;
    int get_number_of_vertex_neighbours(int vid) const;
    int get_neighbour_id_vertex(int vid, int nid) const;

protected: // light mesh
    Vertex* get_vertex(int vid);
    const Vertex* get_vertex(int vid) const;
    Face* get_face(int fid);
    const Face* get_face(int fid) const;

public: /// GeometryBase
     unsigned int getPointsNumber() const;
     Vec3 getVertexPosition(unsigned int i) const;
     void setVertexPosition(unsigned int i, const Vec3& pos);
     Vec3 getVertexNormal(unsigned int i) const;
     void setVertexNormal(unsigned int i, const Vec3& nor);
     Vec4 getVertexColor(unsigned int i) const = 0;
     void setVertexColor(unsigned int i, const Vec4& color);
     bool isVertexSelected(unsigned int i) const;
     void setVertexSelected(unsigned int i, bool isSelected);

     unsigned int getEdgesNumber() const {return 0;}
     EdgeIndices getEdgeVertexIndices(unsigned int i) const {return EdgeIndices();}
     Vec4 getEdgeColor(unsigned int i) const {return Vec4();}
     void setEdgeColor(unsigned int i, const Vec4& color) {}
     bool isEdgeSelected(unsigned int i) const {return false;}
     void setEdgeSelected(unsigned int i, bool isSelected) {}

     unsigned int getFacesNumber() const;
     unsigned int getFaceValence(unsigned int i) const {return 3;}
     TriIndices getFaceTriIndices(unsigned int i) const;
     QuatIndices getFaceQuatIndices(unsigned int i) const {return QuatIndices();}
     PolygonIndices getFacePolygonIndices(unsigned int i) const {return PolygonIndices();}
     bool isFaceSelected(unsigned int i) const;
     void setFaceSelected(unsigned int i, bool isSelected);
protected:
    std::vector<Vertex> vertices_;
    std::vector<Face> faces_;

};
}
#endif // LIGHTMESH_H

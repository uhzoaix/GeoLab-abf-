#include "LightMesh.h"

namespace GCL {
LightMesh::LightMesh()
{

}

LightMesh::~LightMesh()
{

}

void LightMesh::clear()
{
    vertices_.clear();
    faces_.clear();
}

int LightMesh::add_point(const Vec3 &pos)
{
    Vertex v;
    v.pos_ = pos;
    v.normal_ = Vec3(0.0);
    v.id_ = vertices_.size();
    v.neighbors_.clear();
    vertices_.push_back(v);
    return v.id_;
}

int LightMesh::add_face(const Vec3i &indices)
{
    for(int i=0; i < 3; i++)
    {
        if(indices[i] >= (int)vertices_.size())
        {
            return -1;
        }
    }
    Face f;
    f.id_ = faces_.size();
    f.indices_ = indices;
    for(int i=0; i < 3; i++)
    {
        Vertex* vertex = get_vertex(indices[i]);
        vertex->add_neighbor(indices[(i+1)%3]);
        vertex->add_neighbor(indices[(i+2)%3]);
    }

    return f.id_;
}

void LightMesh::update_normals()
{
    for(size_t i=0; i < faces_.size(); i++)
    {
        Face* f = &faces_[i];
        Vec3 v[3];
        for(int j=0; j < 3; j++)
        {
            v[j] = get_vertex(f->indices_[j])->pos_;
        }
        Vec3 vn = (v[1] - v[0]) ^ (v[2] - v[1]);
        vn.Normalize();
        f->normal_ = vn;
    }

    for(size_t i=0; i < vertices_.size(); i++)
    {
        int fnum =  get_number_of_vertex_face(i);
        for(int j=0; j < fnum; j++)
        {

        }
    }
}

int LightMesh::get_number_of_vertex_face(int vid) const
{
    const Vertex* vertex = get_vertex(vid);
    if(!vertex) return -1;
    return vertex->face_ids_.size();
}

int LightMesh::get_face_id_of_vertex(int vid, int index) const
{
    const Vertex* vertex = get_vertex(vid);
    if(!vertex) return -1;
    if(index<0 || index >= (int)vertex->face_ids_.size()) return -1;
    return vertex->face_ids_[index];
}

int LightMesh::get_number_of_vertex_neighbours(int vid) const
{
    const Vertex* vertex = get_vertex(vid);
    if(!vertex) return -1;
    return vertex->neighbors_.size();
}

int LightMesh::get_neighbour_id_vertex(int vid, int nid) const
{
    const Vertex* vertex = get_vertex(vid);
    if(!vertex) return -1;
    if(nid<0 || nid >= (int)vertex->neighbors_.size()) return -1;
    return vertex->neighbors_[nid];
}

LightMesh::Vertex *LightMesh::get_vertex(int vid)
{
    if(vid < 0 || vid >= (int)vertices_.size())  return NULL;
    return &vertices_[vid];
}

const LightMesh::Vertex *LightMesh::get_vertex(int vid) const
{
    if(vid < 0 || vid >= (int)vertices_.size())  return NULL;
    return &vertices_[vid];
}

LightMesh::Face *LightMesh::get_face(int fid)
{
    if(fid < 0 || fid >= (int)faces_.size()) return NULL;
    return &faces_[fid];
}

const LightMesh::Face *LightMesh::get_face(int fid) const
{
    if(fid < 0 || fid >= (int)faces_.size()) return NULL;
    return &faces_[fid];
}

unsigned int LightMesh::getPointsNumber() const
{
    return vertices_.size();
}

Vec3 LightMesh::getVertexPosition(unsigned int i) const
{
    return vertices_[i].pos_;
}

void LightMesh::setVertexPosition(unsigned int i, const Vec3 &pos)
{
    vertices_[i].pos_ = pos;
}

Vec3 LightMesh::getVertexNormal(unsigned int i) const
{
    return vertices_[i].normal_;
}

void LightMesh::setVertexNormal(unsigned int i, const Vec3 &nor)
{
    vertices_[i].normal_ = nor;
}

void LightMesh::setVertexColor(unsigned int i, const Vec4 &color)
{
    vertices_[i].color_ = color;
}

bool LightMesh::isVertexSelected(unsigned int i) const
{
    return vertices_[i].is_selected_;
}

void LightMesh::setVertexSelected(unsigned int i, bool isSelected)
{
    vertices_[i].is_selected_ = isSelected;
}

unsigned int LightMesh::getFacesNumber() const
{
    return faces_.size();
}

GeometryBase::TriIndices LightMesh::getFaceTriIndices(unsigned int i) const
{
    return faces_[i].indices_;
}

bool LightMesh::isFaceSelected(unsigned int i) const
{
    return faces_[i].is_selected_;
}

void LightMesh::setFaceSelected(unsigned int i, bool isSelected)
{
    faces_[i].is_selected_ = isSelected;
}

Vec4 LightMesh::getVertexColor(unsigned int i) const
{
    return vertices_[i].color_;
}

}

#ifndef OMESH_H
#define OMESH_H
#include "Core/gcl_global.h"
#include "GCLTraits.h"
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Utils/PropertyContainer.hh>
#include "GeometryBase.h"
#include <vector>
namespace GCL{
class GCL_CORE_SHARED_EXPORT OMesh : public OpenMesh::PolyMesh_ArrayKernelT<GCLTraits>, public GeometryBase
{
public:
    typedef std::vector<VertexHandle> VHandles;
    OMesh();

    bool is_triangle( FaceHandle fh ) const;
    bool is_triangle_mesh() const;
    bool is_flip_ok( EdgeHandle eh ) const;
    bool is_collapse_ok( HalfedgeHandle v0v1 );

    void flip ( EdgeHandle eh );

    /// Edge split (= 2-to-4 split)
    void split(EdgeHandle _eh, VertexHandle _vh);

    /// Face split (= 1-to-3 split, calls corresponding PolyMeshT function).
    void split(FaceHandle _fh, VertexHandle _vh);


public:
    /** The interface for GeometryBase */
     unsigned int getPointsNumber() const {return n_vertices();}
     Vec3 getVertexPosition(unsigned int i) const {return point(vertex_handle(i));}
     void setVertexPosition(unsigned int i, const Vec3& pos) {set_point(vertex_handle(i),pos);}
     Vec3 getVertexNormal(unsigned int i) const {return normal(vertex_handle(i));}
     void setVertexNormal(unsigned int i, const Vec3& nor) {set_normal(vertex_handle(i),nor);}
     Vec4 getVertexColor(unsigned int i) const {return color(vertex_handle(i));}
     void setVertexColor(unsigned int i, const Vec4& color) {set_color(vertex_handle(i),color);}
     bool isVertexSelected(unsigned int i) const {return status(vertex_handle(i)).selected();}
     void setVertexSelected(unsigned int i, bool isSelected) {status(vertex_handle(i)).set_selected(isSelected);}

     unsigned int getEdgesNumber() const {return n_edges();}
     EdgeIndices getEdgeVertexIndices(unsigned int i) const;
     Vec4 getEdgeColor(unsigned int i) const {return color(edge_handle(i));}
     void setEdgeColor(unsigned int i, const Vec4& color) {set_color(edge_handle(i),color);}
     bool isEdgeSelected(unsigned int i) const {return status(edge_handle(i)).selected();}
     void setEdgeSelected(unsigned int i, bool isSelected) {status(edge_handle(i)).set_selected(isSelected);}

     unsigned int getFacesNumber() const {return n_faces();}
     unsigned int getFaceValence(unsigned int i) const {return valence(face_handle(i));}
     TriIndices getFaceTriIndices(unsigned int i) const;
     QuatIndices getFaceQuatIndices(unsigned int i) const;
     PolygonIndices getFacePolygonIndices(unsigned int i) const;
     bool isFaceSelected(unsigned int i) const {return status(face_handle(i)).selected();}
     void setFaceSelected(unsigned int i, bool isSelected) {status(face_handle(i)).set_selected(isSelected);}

     void MeshNormalize(GCL::Scalar sc = 1);
public:
    // Rewrite the following functions to help the IDEs (e.g. Qt Creator) to autocompletes the functions
    // To use the original functions in OpenMesh, just delete the  last "_";

    const Point& point_(VertexHandle _vh) const {return point(_vh);}
    Point& point_(VertexHandle _vh) {return point(_vh);}

    const Normal& normal_(VertexHandle _vh) const {return normal(_vh);}
    void set_normal_ (VertexHandle _vh, const Normal &_n) {set_normal(_vh,_n);}
    const Color& color_(VertexHandle _vh) const {return color(_vh);}
    void set_color_(VertexHandle _vh, const Color &_c) {set_color(_vh,_c);}

    const TexCoord2D& texcoord2D_(VertexHandle _vh) const {return texcoord2D(_vh);}
    void set_texcoord2D_(VertexHandle _vh, const  TexCoord2D& _t) {set_texcoord2D(_vh,_t);}

    const Color& color_(EdgeHandle _eh) const {return color(_eh);}
    void set_color_(EdgeHandle _eh,const Color& _c) {set_color(_eh,_c);}

    const Color& color_(FaceHandle _fh) const {return color(_fh);}
    void set_color_(FaceHandle _eh,const Color& _c) {set_color(_eh,_c);}

    const Normal& normal_(FaceHandle _fh) const {return normal(_fh);}
    void set_normal_ (FaceHandle _vh, const Normal &_n) {set_normal(_vh,_n);}

    int valence_(VertexHandle _vh) const {return valence(_vh);}
    VertexHandle 	handle_ (const Vertex &_v) const {return handle(_v);}

    HalfedgeHandle 	handle_(const Halfedge &_he) const {return handle(_he);}

    EdgeHandle 	handle_(const Edge &_e) const {return handle(_e);}

    FaceHandle 	handle_(const Face &_f) const {return handle(_f);}

    bool 	is_valid_handle_ (VertexHandle _vh) const {return is_valid_handle(_vh);}

    bool 	is_valid_handle_(HalfedgeHandle _heh) const {return is_valid_handle(_heh);}

    bool 	is_valid_handle_(EdgeHandle _eh) const {return is_valid_handle(_eh);}

    bool 	is_valid_handle_(FaceHandle _fh) const {return is_valid_handle(_fh);}


    VertexHandle 	vertex_handle_(unsigned int _i) const {return vertex_handle(_i);}

    HalfedgeHandle 	halfedge_handle_(unsigned int _i) const {return halfedge_handle(_i);}

    EdgeHandle 	edge_handle_(unsigned int _i) const {return edge_handle(_i);}

    FaceHandle 	face_handle_(unsigned int _i) const {return face_handle(_i);}

    VertexHandle 	new_vertex_() {return new_vertex();}

    HalfedgeHandle 	new_edge_(VertexHandle _start_vh, VertexHandle _end_vh) {return new_edge(_start_vh,_end_vh);}

    FaceHandle 	new_face_() {return new_face();}

    FaceHandle 	new_face_(const Face &_f) {return new_face(_f);}

    void 	resize_(size_t _n_vertices, size_t _n_edges, size_t _n_faces) {resize(_n_vertices,_n_edges,_n_faces);}

    void 	reserve_(size_t _n_vertices, size_t _n_edges, size_t _n_faces) {reserve(_n_vertices,_n_edges,_n_faces);}

    void 	clear_() { clear();}

    void 	clean_() { clean();}

    size_t 	n_vertices_() const {return n_vertices();}

    size_t 	n_halfedges_() const {return n_halfedges();}

    size_t 	n_edges_() const {return n_edges();}

    size_t 	n_faces_() const {return n_faces();}

    bool 	vertices_empty_() const {return vertices_empty();}

    bool 	halfedges_empty_() const {return halfedges_empty();}

    bool 	edges_empty_() const {return edges_empty();}

    bool 	faces_empty_() const {return faces_empty();}

    HalfedgeHandle 	halfedge_handle_(VertexHandle _vh) const {return halfedge_handle(_vh);}

    void 	set_halfedge_handle_(VertexHandle _vh, HalfedgeHandle _heh) {set_halfedge_handle(_vh,_heh);}

    bool 	is_isolated_(VertexHandle _vh) const {return is_isolated(_vh);}

    void 	set_isolated_(VertexHandle _vh) {set_isolated(_vh);}

    bool    is_boundary_(VertexHandle _vh) const {return is_boundary(_vh);}

    unsigned int 	delete_isolated_vertices_() {return delete_isolated_vertices();}

    /// Get the vertex the halfedge points to.
    VertexHandle 	to_vertex_handle_(HalfedgeHandle _heh) const {return to_vertex_handle(_heh);}

    VertexHandle 	from_vertex_handle_(HalfedgeHandle _heh) const {return from_vertex_handle(_heh);}

    void 	set_vertex_handle_(HalfedgeHandle _heh, VertexHandle _vh) { set_vertex_handle(_heh,_vh);}

    FaceHandle 	face_handle_ (HalfedgeHandle _heh) const {return face_handle(_heh);}

    void 	set_face_handle_ (HalfedgeHandle _heh, FaceHandle _fh) {set_face_handle(_heh,_fh);}

    void 	set_boundary_(HalfedgeHandle _heh) {set_boundary(_heh);}

    bool 	is_boundary_(HalfedgeHandle _heh) const {return is_boundary(_heh);}

    bool    is_boundary_(FaceHandle _fh) const {return is_boundary(_fh);}
    FaceHandle add_face_(const VHandles& _indices);
    HalfedgeHandle 	next_halfedge_handle_ (HalfedgeHandle _heh) const {return next_halfedge_handle(_heh); }

    void 	set_next_halfedge_handle_ (HalfedgeHandle _heh, HalfedgeHandle _nheh) {set_next_halfedge_handle(_heh,_nheh);}

    void 	set_prev_halfedge_handle_ (HalfedgeHandle _heh, HalfedgeHandle _pheh) {set_prev_halfedge_handle(_heh,_pheh);}

    HalfedgeHandle 	prev_halfedge_handle_(HalfedgeHandle _heh) const {return prev_halfedge_handle(_heh);}

    HalfedgeHandle 	opposite_halfedge_handle_ (HalfedgeHandle _heh) const {return opposite_halfedge_handle(_heh);}

    /// Counter-clockwise rotate the given halfedge around its from vertex.
    HalfedgeHandle 	ccw_rotated_halfedge_handle_(HalfedgeHandle _heh) const {return ccw_rotated_halfedge_handle(_heh);}

    /// Clockwise  rotate the given around  its from vertex
    HalfedgeHandle 	cw_rotated_halfedge_handle_(HalfedgeHandle _heh) const {return cw_rotated_halfedge_handle(_heh);}

    HalfedgeHandle 	halfedge_handle_(EdgeHandle _eh, unsigned int _i) const {return halfedge_handle(_eh,_i);}

    EdgeHandle 	edge_handle_(HalfedgeHandle _heh) const {return edge_handle(_heh);}

    HalfedgeHandle 	halfedge_handle_(FaceHandle _fh) const {return halfedge_handle(_fh);}

   void 	set_halfedge_handle_ (FaceHandle _fh, HalfedgeHandle _heh) {set_halfedge_handle(_fh,_heh);}

    const OpenMesh::Attributes::StatusInfo & 	status_(VertexHandle _vh) const {return status(_vh);}

    OpenMesh::Attributes::StatusInfo & 	status_(VertexHandle _vh) {return status(_vh);}

    const OpenMesh::Attributes::StatusInfo & 	status_(HalfedgeHandle _hh) const {return status(_hh);}

    OpenMesh::Attributes::StatusInfo & 	status_(HalfedgeHandle _hh) {return status(_hh);}

    const OpenMesh::Attributes::StatusInfo & 	status_(EdgeHandle _eh) const {return status(_eh);}

    OpenMesh::Attributes::StatusInfo & 	status_(EdgeHandle _eh) {return status(_eh);}

    const OpenMesh::Attributes::StatusInfo & 	status_ (FaceHandle _fh) const {return status(_fh);}

    OpenMesh::Attributes::StatusInfo & 	status_ (FaceHandle _fh) {return status(_fh);}

    /// Vertex And Face circulator

    /// vertex-vertex circulaor
    /// Use it like this:   for (vv_it = vv_iter(v0); vv_it; ++vv_it)
    VertexVertexIter vv_iter_(VertexHandle _vh) {return vv_iter(_vh);}

    /// vertex-incomming halfedge circulator
    VertexIHalfedgeIter vih_iter_(VertexHandle _vh) {return vih_iter(_vh);}

    /// vertex-outgoing halfedge circulator
    VertexOHalfedgeIter voh_iter_(VertexHandle _vh) {return voh_iter(_vh);}

    /// vertex-edge circulator
    VertexEdgeIter ve_iter_(VertexHandle _vh) {return ve_iter(_vh);}

    /// Vertex-face circulator
    VertexFaceIter vf_iter_(VertexHandle _vh) {return  vf_iter(_vh); }

    ///  Const Vertex circulators
    ConstVertexVertexIter cvv_iter_(VertexHandle _vh) const {return cvv_iter(_vh);}
    ConstVertexIHalfedgeIter cvih_iter_(VertexHandle _vh) const {return cvih_iter(_vh);}
    ConstVertexOHalfedgeIter cvoh_iter_(VertexHandle _vh) const {return cvoh_iter(_vh);}
    ConstVertexEdgeIter cve_iter_(VertexHandle _vh) const {return cve_iter(_vh);}
    ConstVertexFaceIter cvf_iter_(VertexHandle _vh) {return  cvf_iter(_vh); }

    FaceVertexIter fv_iter_(FaceHandle _fh) {return fv_iter(_fh);}
    FaceHalfedgeIter fh_iter_(FaceHandle _fh) {return fh_iter(_fh);}
    FaceEdgeIter fe_iter_(FaceHandle _fh) {return fe_iter(_fh);}
    FaceFaceIter ff_iter_(FaceHandle _fh) {return ff_iter(_fh);}
    ConstFaceVertexIter cfv_iter_(FaceHandle _fh) const{return cfv_iter(_fh);}
    ConstFaceHalfedgeIter cfh_iter_(FaceHandle _fh) const {return cfh_iter(_fh);}
    ConstFaceEdgeIter cfe_iter_(FaceHandle _fh) const{return cfe_iter(_fh);}
    ConstFaceFaceIter cff_iter_(FaceHandle _fh) const {return cff_iter(_fh);}



    ///    Begin iterator for vertices.
    VertexIter 	vertices_begin_() {return vertices_begin();}
    ///    Const begin iterator for vertices.
    ConstVertexIter vertices_begin_() const {return vertices_begin();}
    ///    End iterator for vertices.
    VertexIter 	vertices_end_() {return vertices_end();}
    ///    Const end iterator for vertices.
    ConstVertexIter vertices_end_() const {return vertices_end();}
    ///    Begin iterator for halfedges.
    HalfedgeIter 	halfedges_begin_() {return halfedges_begin();}
    ///    Const begin iterator for halfedges.
    ConstHalfedgeIter 	halfedges_begin_() const {return halfedges_begin();}
    ///    End iterator for halfedges.
    HalfedgeIter halfedges_end_()  {return halfedges_end();}
    ///    Const end iterator for halfedges.
    ConstHalfedgeIter 	halfedges_end_() const {return halfedges_end();}
    ///    Begin iterator for edges.
    EdgeIter 	edges_begin_ ()  {return edges_begin();}
    ///    Const begin iterator for edges.
    ConstEdgeIter 	edges_begin_ () const {return edges_begin();}
    ///    End iterator for edges.
    EdgeIter edges_end_ () {return edges_end();}
    ///    Const end iterator for edges.
    ConstEdgeIter 	edges_end_ () const {return edges_end();}
    ///    Begin iterator for faces.
    FaceIter 	faces_begin_() {return faces_begin();}
    ///    Const begin iterator for faces.
    ConstFaceIter 	faces_begin_() const {return faces_begin();}
    ///    End iterator for faces.
    FaceIter 	faces_end_() {return  faces_end();}
    ///    Const end iterator for faces.
    ConstFaceIter 	faces_end_() const {return faces_end(); }

};
}

#endif // OMESH_H

#include "OMesh.h"
namespace GCL {
OMesh::OMesh()
{
}


bool OMesh::is_triangle( OpenMesh::FaceHandle fh ) const{
  return valence(fh)==3;
}

bool OMesh::is_triangle_mesh() const{
  for(OMesh::ConstFaceIter fI=faces_begin(); fI!=faces_end(); ++fI)
    if(!is_triangle(FaceHandle(*fI))) return false;
  return true;
}


bool OMesh::is_flip_ok(EdgeHandle _eh) const
{
  // boundary edges cannot be flipped
  if (is_boundary(_eh)) return false;

  // get Halfedge handles
  HalfedgeHandle hh = halfedge_handle(_eh, 0);
  HalfedgeHandle oh = halfedge_handle(_eh, 1);

  // check if neighbors are triangles
  if( !is_triangle(face_handle(hh)) || !is_triangle(face_handle(oh))) return false;

  // check if the flipped edge is already present
  // in the mesh
  VertexHandle ah = to_vertex_handle(next_halfedge_handle(hh));
  VertexHandle bh = to_vertex_handle(next_halfedge_handle(oh));

  if (ah == bh)   // this is generally a bad sign !!!
    return false;

  for (ConstVertexVertexIter vvi(*this, ah); vvi; ++vvi)
    if (vvi.handle() == bh)
      return false;

  return true;
}



bool OMesh::is_collapse_ok(HalfedgeHandle v0v1)
{
  HalfedgeHandle  v1v0(opposite_halfedge_handle(v0v1));
  VertexHandle    v0(to_vertex_handle(v1v0));
  VertexHandle    v1(to_vertex_handle(v0v1));

  // are vertices already deleted ?
  if (status(v0).deleted() || status(v1).deleted())
    return false;

  VertexHandle    vl, vr;
  HalfedgeHandle  h1, h2;

  // the edges v1-vl and vl-v0 must not be both boundary edges
  if (!is_boundary(v0v1))
  {
    vl = to_vertex_handle(next_halfedge_handle(v0v1));

    h1 = next_halfedge_handle(v0v1);
    h2 = next_halfedge_handle(h1);
    if (is_boundary(opposite_halfedge_handle(h1)) &&
        is_boundary(opposite_halfedge_handle(h2)))
    {
      return false;
    }
  }


  // the edges v0-vr and vr-v1 must not be both boundary edges
  if (!is_boundary(v1v0))
  {
    vr = to_vertex_handle(next_halfedge_handle(v1v0));

    h1 = next_halfedge_handle(v1v0);
    h2 = next_halfedge_handle(h1);
    if (is_boundary(opposite_halfedge_handle(h1)) &&
        is_boundary(opposite_halfedge_handle(h2)))
      return false;
  }


  // if vl and vr are equal or both invalid -> fail
  if (vl == vr) return false;


  VertexVertexIter  vv_it;


  // test intersection of the one-rings of v0 and v1
  for (vv_it = vv_iter(v0); vv_it; ++vv_it)
    status(vv_it).set_tagged(false);

  for (vv_it = vv_iter(v1); vv_it; ++vv_it)
    status(vv_it).set_tagged(true);

  for (vv_it = vv_iter(v0); vv_it; ++vv_it)
    if (status(vv_it).tagged() && vv_it.handle() != vl && vv_it.handle() != vr)
      return false;



  // edge between two boundary vertices should be a boundary edge
  if ( is_boundary(v0) && is_boundary(v1) &&
       !is_boundary(v0v1) && !is_boundary(v1v0))
    return false;

  // passed all tests
  return true;
}

void OMesh::flip(EdgeHandle _eh)
{
  // CAUTION : Flipping a halfedge may result in
  // a non-manifold mesh, hence check for yourself
  // whether this operation is allowed or not!
  assert(is_flip_ok(_eh));//let's make it sure it is actually checked
  assert(!is_boundary(_eh));

  HalfedgeHandle a0 = halfedge_handle(_eh, 0);
  HalfedgeHandle b0 = halfedge_handle(_eh, 1);

  // check if neighbors are triangles
  assert( is_triangle(face_handle(a0)) && is_triangle(face_handle(b0)));

  HalfedgeHandle a1 = next_halfedge_handle(a0);
  HalfedgeHandle a2 = next_halfedge_handle(a1);

  HalfedgeHandle b1 = next_halfedge_handle(b0);
  HalfedgeHandle b2 = next_halfedge_handle(b1);

  VertexHandle   va0 = to_vertex_handle(a0);
  VertexHandle   va1 = to_vertex_handle(a1);

  VertexHandle   vb0 = to_vertex_handle(b0);
  VertexHandle   vb1 = to_vertex_handle(b1);

  FaceHandle     fa  = face_handle(a0);
  FaceHandle     fb  = face_handle(b0);

  set_vertex_handle(a0, va1);
  set_vertex_handle(b0, vb1);

  set_next_halfedge_handle(a0, a2);
  set_next_halfedge_handle(a2, b1);
  set_next_halfedge_handle(b1, a0);

  set_next_halfedge_handle(b0, b2);
  set_next_halfedge_handle(b2, a1);
  set_next_halfedge_handle(a1, b0);

  set_face_handle(a1, fb);
  set_face_handle(b1, fa);

  set_halfedge_handle(fa, a0);
  set_halfedge_handle(fb, b0);

  if (halfedge_handle(va0) == b0)
    set_halfedge_handle(va0, a1);
  if (halfedge_handle(vb0) == a0)
    set_halfedge_handle(vb0, b1);
}

void OMesh::split(FaceHandle _fh, VertexHandle _vh)
{
    PolyConnectivity::split(_fh, _vh);
}


GeometryBase::EdgeIndices OMesh::getEdgeVertexIndices(unsigned int i) const
{
    HalfedgeHandle handle  = halfedge_handle(edge_handle(i),1);
    GeometryBase::EdgeIndices v;
    v[0] = to_vertex_handle_(handle).idx();
    v[1] = from_vertex_handle_(handle).idx();
    return v;
}

GeometryBase::TriIndices OMesh::getFaceTriIndices(unsigned int i) const
{
    GeometryBase::TriIndices v;
    int j = 0;
    for(ConstFaceVertexIter cfv_itr(cfv_iter(face_handle(i)));cfv_itr; cfv_itr++)
    {
        v[j++] = (*cfv_itr).idx();
        if(j >= 3) break;
    }
    return v;
}

GeometryBase::QuatIndices OMesh::getFaceQuatIndices(unsigned int i) const
{
    GeometryBase::QuatIndices v;
    int j = 0;
    for(ConstFaceVertexIter cfv_itr(cfv_iter(face_handle(i)));cfv_itr; cfv_itr++)
    {
        v[j++] = (*cfv_itr).idx();
        if(j >= 4) break;
    }
    return v;
}

GeometryBase::PolygonIndices OMesh::getFacePolygonIndices(unsigned int i) const
{
    GeometryBase::PolygonIndices v(getFaceValence(i));
    int j = 0;
    for(ConstFaceVertexIter cfv_itr(cfv_iter(face_handle(i)));cfv_itr; cfv_itr++)
    {
        v[j++] = (*cfv_itr).idx();
    }
    return v;
}

void OMesh::MeshNormalize(GCL::Scalar sc)
{
    Vec3 vmin,vmax;
    int flag = 0;
    for(VertexIter itr = vertices_begin_(); itr != vertices_end_(); itr++)
    {
        if(!flag)
        {
            vmin = vmax = point(*itr);
            flag = 1;
        }
        else
        {
            vmin.min(point(*itr));
            vmax.max(point(*itr));
        }
    }
    Vec3 center = (vmax + vmin) * 0.5;
    Scalar len = (vmax - vmin).length() / 2;
    for(VertexIter itr = vertices_begin_(); itr != vertices_end_(); itr++)
    {
        point(*itr) = (point(*itr) - center) * (sc / len);
    }

}

OpenMesh::FaceHandle OMesh::add_face_(const VHandles &_indices)
{
    FaceHandle fh;

    if (_indices.size() > 2)
    {
      VHandles::const_iterator it, it2, end(_indices.end());


      // test for valid vertex indices
      for (it=_indices.begin(); it!=end; ++it)
      {
        if (!is_valid_handle(*it))
        {
          DebugLog<< "add_face: Face contains invalid vertex index\n";
          return fh;
        }
      }

      // don't allow double vertices
      for (it=_indices.begin(); it!=end; ++it)
      {
        for (it2=it+1; it2!=end; ++it2)
        {
          if (*it == *it2)
          {
            DebugLog << "ImporterT: Face has equal vertices\n";
//            failed_faces_.push_back(_indices);
            return fh;
          }
        }
      }


      // try to add face
      fh = add_face(_indices);
      if (!fh.is_valid())
      {
//        failed_faces_.push_back(_indices);
        return fh;
      }
    }
    return fh;
}



void OMesh::split(EdgeHandle _eh, VertexHandle _vh)
{
  HalfedgeHandle h0 = halfedge_handle(_eh, 0);
  HalfedgeHandle o0 = halfedge_handle(_eh, 1);

  VertexHandle   v2 = to_vertex_handle(o0);

  HalfedgeHandle e1 = new_edge(_vh, v2);
  HalfedgeHandle t1 = opposite_halfedge_handle(e1);

  FaceHandle     f0 = face_handle(h0);
  FaceHandle     f3 = face_handle(o0);

  set_halfedge_handle(_vh, h0);
  set_vertex_handle(o0, _vh);

  if (!is_boundary(h0))
  {
    assert( is_triangle(face_handle(h0)) );

    HalfedgeHandle h1 = next_halfedge_handle(h0);
    HalfedgeHandle h2 = next_halfedge_handle(h1);

    VertexHandle v1 = to_vertex_handle(h1);

    HalfedgeHandle e0 = new_edge(_vh, v1);
    HalfedgeHandle t0 = opposite_halfedge_handle(e0);

    FaceHandle f1 = new_face();
    set_halfedge_handle(f0, h0);
    set_halfedge_handle(f1, h2);

    set_face_handle(h1, f0);
    set_face_handle(t0, f0);
    set_face_handle(h0, f0);

    set_face_handle(h2, f1);
    set_face_handle(t1, f1);
    set_face_handle(e0, f1);

    set_next_halfedge_handle(h0, h1);
    set_next_halfedge_handle(h1, t0);
    set_next_halfedge_handle(t0, h0);

    set_next_halfedge_handle(e0, h2);
    set_next_halfedge_handle(h2, t1);
    set_next_halfedge_handle(t1, e0);
  }
  else
  {
    set_next_halfedge_handle(prev_halfedge_handle(h0), t1);
    set_next_halfedge_handle(t1, h0);
    // halfedge handle of _vh already is h0
  }


  if (!is_boundary(o0))
  {
    assert( is_triangle(face_handle(o0)) );

    HalfedgeHandle o1 = next_halfedge_handle(o0);
    HalfedgeHandle o2 = next_halfedge_handle(o1);

    VertexHandle v3 = to_vertex_handle(o1);

    HalfedgeHandle e2 = new_edge(_vh, v3);
    HalfedgeHandle t2 = opposite_halfedge_handle(e2);

    FaceHandle f2 = new_face();
    set_halfedge_handle(f2, o1);
    set_halfedge_handle(f3, o0);

    set_face_handle(o1, f2);
    set_face_handle(t2, f2);
    set_face_handle(e1, f2);

    set_face_handle(o2, f3);
    set_face_handle(o0, f3);
    set_face_handle(e2, f3);

    set_next_halfedge_handle(e1, o1);
    set_next_halfedge_handle(o1, t2);
    set_next_halfedge_handle(t2, e1);

    set_next_halfedge_handle(o0, e2);
    set_next_halfedge_handle(e2, o2);
    set_next_halfedge_handle(o2, o0);
  }
  else
  {
    set_next_halfedge_handle(e1, next_halfedge_handle(o0));
    set_next_halfedge_handle(o0, e1);
    set_halfedge_handle(_vh, e1);
  }

  if (halfedge_handle(v2) == h0)
    set_halfedge_handle(v2, t1);
}

}


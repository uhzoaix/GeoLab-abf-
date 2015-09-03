#ifndef GCL_BASIC_MEASURING_H
#define GCL_BASIC_MEASURING_H
#include <Utilities/OpenMeshAlgorithms/gcl_export.h>
#include <Core/DataStructures/OMesh.h>
#include <Core/DataStructures/BarycentricCoordinates.h>

namespace GCL { namespace Utilities {

#ifndef M_PI
	#define M_PI 3.1415926535897931
#endif


class GCL_OPENMESHALGORITHMS_SHARED_EXPORT BasicMeasuring{




private:

  /** private constructor for this static class */
  BasicMeasuring(){}

public:

  //Uniform laplacian at a vertex
  static Vec3 laplacian( OMesh* m, OMesh::VertexHandle vh );

  //Weighted laplacian
  static Vec3 laplacian( OMesh* m, OMesh::VertexHandle vh, OpenMesh::EPropHandleT<Scalar> w_e, OpenMesh::VPropHandleT<Scalar> w_v );

  //Weighted laplacian of a custom property of a Mesh
  template< typename T>
  static void laplacian( OMesh* m, OpenMesh::VPropHandleT<T> F, OpenMesh::EPropHandleT<Scalar> w_e, OpenMesh::VPropHandleT<Scalar> w_v, OpenMesh::VPropHandleT<T> laplacianT ){

    OpenMesh::VPropHandleT<T> result=laplacianT;

    OpenMesh::VPropHandleT<T> intermediate;
    if(F.idx() == laplacianT.idx()){
      m->add_property(intermediate,"v:laplacian::intermediate");
      result = intermediate;
    }

    for( int v=0; v< static_cast<int>(m->n_vertices()); ++v )
    {
      OMesh::VertexHandle vh(v);
      T f=m->property(F,vh);
      T l;
      for(OMesh::ConstVertexOHalfedgeIter vohI=m->cvoh_iter(vh); vohI; ++vohI){
        l += m->property(w_e,m->edge_handle(vohI.handle()))*(m->property(F,m->to_vertex_handle(vohI.handle())) - f );
      }
      l *= m->property(w_v,vh);

      m->property(result,vh) = l;
    }

    if(F.idx() == laplacianT.idx()){
      for( OMesh::VertexIter vI=m->vertices_begin(); vI!=m->vertices_end(); ++vI)
        m->property(laplacianT,vI.handle())=m->property(result,vI.handle());

      m->remove_property(intermediate);
    }
  }

  //Laplacian Matrix acting on a n-dimensional vertex property
  template<class M>
  static void Laplacian(  M& L, OMesh* m,  OpenMesh::EPropHandleT<Scalar> w_e , OpenMesh::VPropHandleT<Scalar> w_v, int n )
  {
	for( unsigned int v=0; v<m->n_vertices(); ++v )
    {
      OMesh::VertexHandle vh(v);
      Scalar wv = m->property(w_v,vh);
      Scalar sumWe = 0.;
      for(OMesh::ConstVertexVertexIter vvI=m->cvv_iter(vh); vvI; ++vvI)
      {
        OMesh::VertexHandle vj=vvI.handle();
        Scalar we=m->property(w_e,m->edge_handle(vvI.current_halfedge_handle()));
        Scalar l=we*wv;

        for(int d=0;d<n;d++)
		  L(v*n+d,(vj.idx())*n+d) += l;

        sumWe += we;
      }

      for(int d=0;d<n;d++)
		L(v*n+d,v*n+d) += -sumWe*wv;
    }
  }

  //Cotan laplacian, as this allocates and deallocates the weight, this is not very efficient, but convinient
  template<class M>
  static void CotanLaplacian( M& L, OMesh * m, int n=1){

	  OpenMesh::VPropHandleT<Scalar> vw;
	  OpenMesh::EPropHandleT<Scalar> ew;
	  m->add_property(vw,"CotanLaplacian::vw");
	  m->add_property(ew,"CotanLaplacian::ew");

	  areaWeights(m,vw);
	  cotangentWeights(m,ew);
	  Laplacian<M>(L,m,ew,vw,n);

	  m->remove_property(vw);
	  m->remove_property(ew);
  }

  //Uniform Laplacian Matrix acting on a n-dimensional vertex property
  template<class M>
  static void uniformLaplacian(  M& L, OMesh* m, int n )
  {
	for( int v=0; v< static_cast<int>(m->n_vertices()); ++v )
    {
      OMesh::VertexHandle vh(v);
      int val = m->valence(vh);
      for(OMesh::ConstVertexVertexIter vvI=m->cvv_iter(vh); vvI; ++vvI)
      {
        for(int d=0;d<n;d++)
		  L(v*n+d,(vvI.handle().idx())*n+d) = 1.0/double(val);
      }

      for(int d=0;d<n;d++)
		L(v*n+d,v*n+d) = -1.0;
    }

  }

  //Compute cotangent weights
  static void cotangentWeights( OMesh* m, OpenMesh::EPropHandleT<Scalar> w);

  //w has to be of size m->n_edges()!
  static void cotangentWeights( OMesh* m, std::vector<Scalar>& w );

  //Compute voronoi area weights
  static void areaWeights( OMesh* m, OpenMesh::VPropHandleT<Scalar> w );

  //w has to be of size m->n_vertices(), areaWeights are ADDED, so most probably you want to set entries to 0.
  static void areaWeights( OMesh* m, std::vector<Scalar>& w );

  //Computes the sum of Edge Weights around vh
  static Scalar sumOfEdgeWeights( const OMesh* m, OpenMesh::EPropHandleT<Scalar> w, OpenMesh::VertexHandle vh);

  ////some usefull measures and functions from Keenans Trivial Connection Code
  static Scalar circumradius( const OMesh* m, const OpenMesh::FaceHandle _face);                       // returns circumradius
  static Scalar inradius( const OMesh* m, const OpenMesh::FaceHandle _face);                           // returns inradius
  static Vec3 barycenter( const OMesh* m, const OpenMesh::FaceHandle _face);                        // returns mean of vertex positions
  static Vec3 midpoint( const OMesh* m, const OpenMesh::EdgeHandle _edge );                         // returns the midpoint of an edge
  static Vec3 interpolate( const OMesh* m, const OpenMesh::HalfedgeHandle _he, Scalar t);           // return the position t of the halfedge, t=0: from(he), t=1: to(he)
  // returns discrete Gaussian curvature given by 2 pi minus sum of tip angle defects.
  template< typename MeshT >
  static Scalar defect( const MeshT* m, const OpenMesh::VertexHandle _vh){
      Scalar sum = 0.;
      OMesh::HalfedgeHandle he = m->halfedge_handle(_vh);
      // iterate over incident triangles
      OMesh::HalfedgeHandle out = he;
      do
      {
         // grab vertices, s.t. p1 is _vh:
         Vec3 p1 = m->point(m->from_vertex_handle(he));//he->from->position;
         Vec3 p2 = m->point(m->from_vertex_handle(m->next_halfedge_handle(he))); //he->next->from->position;
         Vec3 p3 = m->point(m->from_vertex_handle(m->next_halfedge_handle(m->next_halfedge_handle(he)))); //he->next->next->from->position;

         // subtract incident angle from sum
         Vec3 u1 = ( p2 - p1 );
         Vec3 u2 = ( p3 - p1 );
         sum += atan2( (u1^u2).length(), u1*u2 );

         he = m->next_halfedge_handle(m->opposite_halfedge_handle(he)); //he->flip->next;
      }
      while( he != out );

      return 2.*M_PI - sum;
   }

  static Vec3 toVector( const OMesh* m, OpenMesh::HalfedgeHandle _heh);                             // Vector of a halfedge

  static Scalar dualEdgeLength( const OMesh* m, OpenMesh::EdgeHandle _eh );

  static Scalar squaredDistancePointToTriangle(Vec3 & p, const OMesh* m, const OpenMesh::FaceHandle fh, Vec3& projectedPoint, BarycentricCoordinates& bc );
  static Scalar squaredDistancePointToTriangle(Vec3 & _point,Vec3 & _triangleVertexA,Vec3 & _triangleVertexB,Vec3 & _triangleVertexC);
  static Scalar squaredDistancePointToTriangle(Vec3  & _point,Vec3  & _triangleVertexA,Vec3  & _triangleVertexB,Vec3  & _triangleVertexC, Vec3& projectedPoint );
  static Scalar squaredDistancePointToTriangle(Vec3 & _point,Vec3 & _triangleVertexA,Vec3 & _triangleVertexB,Vec3 & _triangleVertexC, Vec3 & _projectedPoint, BarycentricCoordinates& bc );

  static Scalar signedNormalAngleOfAdjacentTriangles(	OMesh::HalfedgeHandle & _he,
                                                OMesh * inputObjectDataStructure);

  static Scalar signedNormalAngleOfAdjacentTriangles(const	Vec3 & _vertex0,
                                              const	Vec3 & _vertex1,
                                              const	Vec3 & _vertex2,
                                              const	Vec3 & _vertex3);

  static void signedNormalAngleOfAdjacentTriangleGradient(const	Vec3 & _vertex0,
                                                   const	Vec3 & _vertex1,
                                                   const	Vec3 & _vertex2,
                                                   const	Vec3 & _vertex3,
                                                   Vec3 & _gradient0,
                                                   Vec3 & _gradient1,
                                                   Vec3 & _gradient2,
                                                   Vec3 & _gradient3);

  static Scalar surfaceArea( const OMesh* m );

  static Scalar triangleSurfaceArea(	const OMesh::FaceHandle & _f1, const OMesh * inputObjectDataStructure);

  static Scalar triangleSurfaceArea(	const Vec3 & pointA,
                              const Vec3 & pointB,
                              const Vec3 & pointC);
  // assuming there is an intersection
  static Scalar triangleSphereIntersectionSurfaceArea(const Vec3 & _sphereCenter,
                                               const Scalar _sphereRadius,
                                               const Vec3 & _trianglePointA,
                                               const Vec3 & _trianglePointB,
                                               const Vec3 & _trianglePointC);

  static Scalar circleSectorArea(Scalar _sphereRadius, Scalar _angle);


  //angle between BA and CA
  static Scalar triangleAngleBAC(const Vec3 & _pointA, const Vec3 & _pointB,const Vec3 & _pointC);

};

}}//namespaces

#endif

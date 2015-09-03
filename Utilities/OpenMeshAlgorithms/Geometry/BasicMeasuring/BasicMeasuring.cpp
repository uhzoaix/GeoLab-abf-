#include "BasicMeasuring.h"
#include <Core/DataStructures/OMesh.h>
#include <cmath>
#include <algorithm>
//#include <Utilities/OpenMeshAlgorithms/Geometry/BasicIntersections/BasicIntersections.h>

namespace GCL { namespace Utilities {

using namespace OpenMesh;

Scalar BasicMeasuring::circleSectorArea(Scalar _sphereRadius, Scalar _angle){
  return _sphereRadius * _sphereRadius * _angle / 2.0;
}

//Uniform laplacian at a vertex
Vec3 BasicMeasuring::laplacian( OMesh* m, OMesh::VertexHandle vh ){

  Vec3 l(0.,0.,0.);
  for(OMesh::ConstVertexVertexIter vvI = m->cvv_iter(vh); vvI; ++vvI){
    l += m->point(vvI);
  }
  l /= static_cast<Scalar>(m->valence(vh));
  l -= m->point(vh);

  return l;
}

//Weighted laplacian
Vec3 BasicMeasuring::laplacian( OMesh* m, OMesh::VertexHandle vh, OpenMesh::EPropHandleT<Scalar> w_e, OpenMesh::VPropHandleT<Scalar> w_v ){

  Vec3 p=m->point(vh);
  Vec3 l(0.,0.,0.);
  for(OMesh::ConstVertexOHalfedgeIter vohI=m->cvoh_iter(vh); vohI; ++vohI){
    l += (m->point(m->to_vertex_handle(vohI.handle())) - p )*m->property(w_e,m->edge_handle(vohI.handle()));
  }
  l *= m->property(w_v,vh);

  return l;
}

//Compute cotangent weights and vertex area weights
void BasicMeasuring::cotangentWeights( OMesh* m, OpenMesh::EPropHandleT<Scalar> w )
{
	 cotangentWeights(m,m->property(w).data_vector());
}

void BasicMeasuring::cotangentWeights(OMesh *m, std::vector<Scalar> &w)
{
	assert(m->n_edges()==w.size());

	OMesh::EdgeIter          e_it, e_end(m->edges_end());
	OMesh::HalfedgeHandle    h0, h1, h2;
	OMesh::VertexHandle      v0, v1;
    Vec3             p0, p1, p2, d0, d1;
	Scalar            we;

	for (e_it=m->edges_begin(); e_it!=e_end; ++e_it)
	{
	  we  = 0.0;

	  h0 = m->halfedge_handle(e_it.handle(), 0);
	  v0 = m->to_vertex_handle(h0);
	  p0 = m->point(v0);

	  h1 = m->halfedge_handle(e_it.handle(), 1);
	  v1 = m->to_vertex_handle(h1);
	  p1 = m->point(v1);

	  if(!m->is_boundary(h0)){
		  h2 = m->next_halfedge_handle(h0);
		  p2 = m->point(m->to_vertex_handle(h2));
		  d0 = (p0 - p2);
		  d1 = (p1 - p2);
          we = ((d0 * d1))/((d0^d1).length());
	  }

	  if(!m->is_boundary(h1)){
		  h2 = m->next_halfedge_handle(h1);
		  p2 = m->point(m->to_vertex_handle(h2));
		  d0 = (p0 - p2);
		  d1 = (p1 - p2);
          we += ((d0 * d1))/((d0 ^ d1).length());
      }

	  w[e_it.handle().idx()] = we;
	}
}


//Compute vornoi area weights
void BasicMeasuring::areaWeights( OMesh* m, OpenMesh::VPropHandleT<Scalar> w )
{
	areaWeights(m,m->property(w).data_vector());
}

void BasicMeasuring::areaWeights(OMesh *m, std::vector<Scalar> &w)
{
	assert(m->n_vertices()==w.size());

	for(OMesh::ConstFaceIter fI=m->faces_begin();fI!=m->faces_end();++fI)
	{
	  //vertices
	  OMesh::ConstFaceVertexIter fvI=m->cfv_iter(fI.handle());
	  const OMesh::VertexHandle v0=fvI;
      const Vec3& p0 = m->point(fvI);  ++fvI;
	  const OMesh::VertexHandle v1=fvI;
      const Vec3& p1 = m->point(fvI);  ++fvI;
	  const OMesh::VertexHandle v2=fvI;
      const Vec3& p2 = m->point(fvI);

	  //edges
      Vec3 e0 = p1-p0;
      Vec3 e1 = p2-p1;
      Vec3 e2 = p0-p2;

	  //cotangents at v0,v1,v2
      Scalar a0 = (e0*-e2)/((e0^-e2).length());
      Scalar a1 = (-e0*e1)/((-e0^e1).length());
      Scalar a2 = (-e1*e2)/((-e1^e2).length());

	  //adding area weights
      w[v0.idx()] += 0.125*(e0.length2()*a1 + e2.length2()*a2);
      w[v1.idx()] += 0.125*(e0.length2()*a0 + e1.length2()*a2);
      w[v2.idx()] += 0.125*(e1.length2()*a1 + e2.length2()*a0);
	}
}



//sum of weights on edges outgoing of a vertex:
Scalar BasicMeasuring::sumOfEdgeWeights( const OMesh* m, OpenMesh::EPropHandleT<Scalar> w, OpenMesh::VertexHandle vh){

  Scalar s=0.;
  for (OMesh::ConstVertexEdgeIter veI=m->cve_iter(vh); veI; ++veI)
    s+=m->property(w,veI);

  return s;
}



////some usefull measures and functions from Keenans Trivial Connection Code:
Scalar BasicMeasuring::circumradius( const OMesh* m, const OpenMesh::FaceHandle _face)
{
  OMesh::ConstFaceVertexIter cfI=m->cfv_iter(_face);
  const Vec3 a(m->point(cfI.handle())); ++cfI;
  const Vec3 b(m->point(cfI.handle())); ++cfI;
  const Vec3 c(m->point(cfI.handle()));
  double u = (a-b).length();
  double v = (b-c).length();
  double w = (c-a).length();

  return (u*v*w)/sqrt((u+v-w)*(u-v+w)*(-u+v+w)*(u+v+w));
}

Scalar BasicMeasuring::inradius( const OMesh* m, const OpenMesh::FaceHandle _face)
{
  OMesh::ConstFaceVertexIter cfI=m->cfv_iter(_face);
  const Vec3 a(m->point(cfI.handle())); ++cfI;
  const Vec3 b(m->point(cfI.handle())); ++cfI;
  const Vec3 c(m->point(cfI.handle()));
  Scalar u = (a-b).length();
  Scalar v = (b-c).length();
  Scalar w = (c-a).length();

  Scalar s = (u+v+w)*0.5;
  return sqrt(((s-u)*(s-v)*(s-w))/s);
}

Vec3 BasicMeasuring::barycenter( const OMesh* m, const OpenMesh::FaceHandle _face)
{
  int n=0;
  Vec3 p(0.,0.,0.);
  for( OMesh::ConstFaceVertexIter cfI=m->cfv_iter(_face); cfI; ++cfI,n++)
    p+=m->point(cfI.handle());
  return p/static_cast<Scalar>(n);
}

Vec3 BasicMeasuring::midpoint( const OMesh* m, const OpenMesh::EdgeHandle _edge )
{
  OMesh::HalfedgeHandle he = m->halfedge_handle(_edge,0);
  return (m->point(m->from_vertex_handle(he))+m->point(m->to_vertex_handle(he)))/2.;
}

Vec3 BasicMeasuring::interpolate( const OMesh* m, const OpenMesh::HalfedgeHandle _he, Scalar t)
{
  return (1.-t)*m->point(m->from_vertex_handle(_he))+t*m->point(m->to_vertex_handle(_he));
}

Vec3 BasicMeasuring::toVector(const OMesh* m,HalfedgeHandle _heh){
  return (m->point(m->to_vertex_handle(_heh))-m->point(m->from_vertex_handle(_heh)));
}

Scalar BasicMeasuring::dualEdgeLength( const OMesh* m, OpenMesh::EdgeHandle _eh ){
	assert(!m->is_boundary(_eh));
    Vec3 mid = midpoint(m,_eh);
    Scalar	l = (mid-barycenter(m,m->face_handle(m->halfedge_handle(_eh,0)))).length();
            l += (mid-barycenter(m,m->face_handle(m->halfedge_handle(_eh,1)))).length();
	return l;
}

Scalar BasicMeasuring::triangleSurfaceArea(const OMesh::FaceHandle & _face, const OMesh * inputObjectDataStructure){

    OMesh::ConstFaceVertexIter fvIt=inputObjectDataStructure->cfv_iter(_face);
    Vec3 pointA = inputObjectDataStructure->point(fvIt);
  Vec3 pointB = inputObjectDataStructure->point(++fvIt);
  Vec3 pointC = inputObjectDataStructure->point(++fvIt);

  Scalar a = (pointA-pointB).length();
  Scalar b = (pointB-pointC).length();
  Scalar c = (pointC-pointA).length();

  //henrons formula:
  Scalar s = .5 * (a+b+c);
  Scalar sqrA = s*(s-a)*(s-b)*(s-c);
  if (sqrA < 0) {
    sqrA = 0;
  }
  return std::sqrt(sqrA);

}

Scalar BasicMeasuring::surfaceArea( const OMesh* m ){
  Scalar area=0.;
  for(OMesh::ConstFaceIter fI=m->faces_begin(); fI!=m->faces_end(); ++fI )
    area += triangleSurfaceArea(fI,m);
  return area;
}

Scalar BasicMeasuring::triangleSurfaceArea(	const Vec3 & pointA, const Vec3 & pointB, const Vec3 & pointC){

  Scalar a = (pointA-pointB).length();
  Scalar b = (pointB-pointC).length();
  Scalar c = (pointC-pointA).length();

  //henrons formula:
  Scalar s = .5 * (a+b+c);
  Scalar sqrA = s*(s-a)*(s-b)*(s-c);
  if (sqrA < 0) {
    sqrA = 0;
  }
  return std::sqrt(sqrA);

}

//Scalar BasicMeasuring::triangleSphereIntersectionSurfaceArea(const Vec3 & _sphereCenter, const Scalar _sphereRadius, const Vec3 & _trianglePointA, const Vec3 & _trianglePointB, const Vec3 & _trianglePointC){

//  std::vector<Vec3> listOfIntersectionsAB;
//  std::vector<Vec3> listOfIntersectionsBC;
//  std::vector<Vec3> listOfIntersectionsCA;
//  bool pointAIsInside, pointBIsInside, pointCIsInside;
//  Scalar area;

//  //call intersection
//  BasicIntersections::sphereTriangleIntersection(_sphereCenter, _sphereRadius, _trianglePointA, _trianglePointB, _trianglePointC, listOfIntersectionsAB, listOfIntersectionsBC, listOfIntersectionsCA, pointAIsInside, pointBIsInside,pointCIsInside, area);

//  return area;

//}

//angle between BA and CA
Scalar BasicMeasuring::triangleAngleBAC(const Vec3  & _pointA,const Vec3  & _pointB,const Vec3  & _pointC){
  Scalar b,c;
  b = (_pointA-_pointC).length();
  c = (_pointA-_pointB).length();
  if (b == 0 || c == 0) {
    return 0; //by definition. TODO: do something else?
  }
  else {
    Scalar a = (_pointB-_pointC).length();
    Scalar cosAngle = (b*b + c*c - a*a)/(2.0*b*c);
    if (cosAngle < -1) {
      cosAngle = -1.;
    }
    else if (cosAngle > 1) {
      cosAngle = 1.;
    }
    return std::acos( cosAngle );
  }
}

// compute signed angle between two incident facets. returns angle between -pi and pi
// returns zero also when edge is boundary edge
Scalar BasicMeasuring::signedNormalAngleOfAdjacentTriangles(	OMesh::HalfedgeHandle & _he,
                                              OMesh * inputObjectDataStructure ) {

  OMesh::FaceHandle f1 = inputObjectDataStructure->face_handle(_he);
  OMesh::FaceHandle f2 = inputObjectDataStructure->face_handle(inputObjectDataStructure->opposite_halfedge_handle(_he));
  if (f1 == OMesh::InvalidFaceHandle || f2 == OMesh::InvalidFaceHandle) {
    return 0;
  }
  else {
    Vec3 normalF1 = inputObjectDataStructure->normal(f1);
    normalF1 = normalF1.normalize();
    Vec3 normalF2 = inputObjectDataStructure->normal(f2);
    normalF2 = normalF2.normalize();
    Vec3 pointA = inputObjectDataStructure->point(inputObjectDataStructure->to_vertex_handle(_he));
    Vec3 pointB = inputObjectDataStructure->point(inputObjectDataStructure->from_vertex_handle(_he));
    Vec3 edge = (pointA-pointB);
    edge = edge.normalize();

    Scalar beta = atan2( ((normalF1^normalF2)*edge), (normalF1*normalF2) );

    assert(beta >= -M_PI && beta <= M_PI);

    return beta;
  }
}

// compute signed angle between two incident facets. returns angle between -pi and pi
// _vertex2-_vertex1 defines the edge
// _vertex0 is on the left side of the edge (looking in the edge-direction)
// _vertex3 is on the right side
Scalar BasicMeasuring::signedNormalAngleOfAdjacentTriangles(const	Vec3 & _vertex0,
                                            const	Vec3 & _vertex1,
                                            const	Vec3 & _vertex2,
                                            const	Vec3 & _vertex3) {


    const Vec3 n1 = ((_vertex1-_vertex0) ^ (_vertex2-_vertex0)).normalize();
    const Vec3 n2 = ((_vertex2-_vertex3) ^ (_vertex1-_vertex3)).normalize();
    const Vec3  e = (_vertex2-_vertex1).normalize();
    const Scalar beta = atan2( ((n1^n2)*e), (n1*n2) );

    assert(beta >= -M_PI && beta <= M_PI);

    return beta;
}


// computes the gradient of the signed angle between two incident faces with respect to the four vertices
// _vertex2-_vertex1 defines the edge
// _vertex0 is on the left side of the edge (looking in the edge-direction)
// _vertex3 is on the right side
// thanks to holy mario for this holy code!
// this method was looked up in robert bridsons "Simulation of clothing with folds and wrinkles" paper
void BasicMeasuring::signedNormalAngleOfAdjacentTriangleGradient(const	Vec3 & _vertex0,
                                            const	Vec3 & _vertex1,
                                            const	Vec3 & _vertex2,
                                            const	Vec3 & _vertex3,
                                            Vec3 & _gradient0,
                                            Vec3 & _gradient1,
                                            Vec3 & _gradient2,
                                            Vec3 & _gradient3) {

  Vec3 n1 = (_vertex1-_vertex0) ^ (_vertex2-_vertex0);
  n1 /= -n1.length2();//has to be scaled with -1 because we consider angle between normals
                      //not anlge between faces as in robert bridson's 2003 cloth paper...

  Vec3 n2 = (_vertex2-_vertex3) ^ (_vertex1-_vertex3);
  n2 /= -n2.length2();

  Vec3  e = (_vertex2-_vertex1);
  float elen = e.length();

  _gradient0 = n1 * elen;
  _gradient3 = n2 * elen;
  e /= elen;
  _gradient1 = ((_vertex0-_vertex2)*e)*n1 + ((_vertex3-_vertex2)*e)*n2;
  _gradient2 = ((_vertex1-_vertex0)*e)*n1 + ((_vertex1-_vertex3)*e)*n2;

}


Scalar BasicMeasuring::squaredDistancePointToTriangle(Vec3 & p, const OMesh* m, const OpenMesh::FaceHandle fh, Vec3& projectedPoint, BarycentricCoordinates& bc ){

    OMesh::ConstFaceVertexIter fvI = m->cfv_iter(fh);
    Vec3 p1 =m->point(fvI.handle()); ++fvI;
    Vec3 p2 =m->point(fvI.handle()); ++fvI;
    Vec3 p3 =m->point(fvI.handle());

    Scalar d =BasicMeasuring::squaredDistancePointToTriangle(p,p1,p2,p3,projectedPoint,bc);
    return d;
  }

//TODO: This first version without projection can probably be implemented a lot more efficiently!!!
Scalar BasicMeasuring::squaredDistancePointToTriangle(Vec3  & _point,Vec3  & _triangleVertexA,Vec3  & _triangleVertexB,Vec3  & _triangleVertexC){

  Vec3 pP;
  BarycentricCoordinates bc;

  return squaredDistancePointToTriangle(_point,_triangleVertexA,_triangleVertexB,_triangleVertexC,pP,bc);
}

Scalar BasicMeasuring::squaredDistancePointToTriangle(Vec3  & _point,Vec3  & _triangleVertexA,Vec3  & _triangleVertexB,Vec3  & _triangleVertexC, Vec3& projectedPoint ){

  BarycentricCoordinates bc;

  return squaredDistancePointToTriangle(_point,_triangleVertexA,_triangleVertexB,_triangleVertexC,projectedPoint,bc);
}



Scalar BasicMeasuring::squaredDistancePointToTriangle(Vec3  & _point,Vec3  & _triangleVertexA,Vec3  & _triangleVertexB,Vec3  & _triangleVertexC, Vec3  & _projectedPoint, BarycentricCoordinates& bc ) {
  Vec3  kDiff = _triangleVertexA - _point;
 Vec3  kEdge0 = _triangleVertexB - _triangleVertexA;
 Vec3  kEdge1 = _triangleVertexC - _triangleVertexA;
 Scalar fA00 = kEdge0.length2();
 Scalar fA01 = (kEdge0 *kEdge1);
 Scalar fA11 = kEdge1.length2();
 Scalar fB0 = (kDiff * kEdge0);
 Scalar fB1 = (kDiff * kEdge1);
 Scalar fC = kDiff.length2();
 Scalar fDet = std::abs(fA00*fA11-fA01*fA01);
 Scalar fS = fA01*fB1-fA11*fB0;
 Scalar fT = fA01*fB0-fA00*fB1;
 Scalar fSqrDistance;

 if (fS + fT <= fDet)
 {
     if (fS < (Scalar)0.0)
     {
         if (fT < (Scalar)0.0)  // region 4
         {
             if (fB0 < (Scalar)0.0)
             {
                 fT = (Scalar)0.0;
                 if (-fB0 >= fA00)
                 {
                     fS = (Scalar)1.0;
                     fSqrDistance = fA00+((Scalar)2.0)*fB0+fC;
                 }
                 else
                 {
                     fS = -fB0/fA00;
                     fSqrDistance = fB0*fS+fC;
                 }
             }
             else
             {
                 fS = (Scalar)0.0;
                 if (fB1 >= (Scalar)0.0)
                 {
                     fT = (Scalar)0.0;
                     fSqrDistance = fC;
                 }
                 else if (-fB1 >= fA11)
                 {
                     fT = (Scalar)1.0;
                     fSqrDistance = fA11+((Scalar)2.0)*fB1+fC;
                 }
                 else
                 {
                     fT = -fB1/fA11;
                     fSqrDistance = fB1*fT+fC;
                 }
             }
         }
         else  // region 3
         {
             fS = (Scalar)0.0;
             if (fB1 >= (Scalar)0.0)
             {
                 fT = (Scalar)0.0;
                 fSqrDistance = fC;
             }
             else if (-fB1 >= fA11)
             {
                 fT = (Scalar)1.0;
                 fSqrDistance = fA11+((Scalar)2.0)*fB1+fC;
             }
             else
             {
                 fT = -fB1/fA11;
                 fSqrDistance = fB1*fT+fC;
             }
         }
     }
     else if (fT < (Scalar)0.0)  // region 5
     {
         fT = (Scalar)0.0;
         if (fB0 >= (Scalar)0.0)
         {
             fS = (Scalar)0.0;
             fSqrDistance = fC;
         }
         else if (-fB0 >= fA00)
         {
             fS = (Scalar)1.0;
             fSqrDistance = fA00+((Scalar)2.0)*fB0+fC;
         }
         else
         {
             fS = -fB0/fA00;
             fSqrDistance = fB0*fS+fC;
         }
     }
     else  // region 0
     {
         // minimum at interior point
         Scalar fInvDet = ((Scalar)1.0)/fDet;
         fS *= fInvDet;
         fT *= fInvDet;
         fSqrDistance = fS*(fA00*fS+fA01*fT+((Scalar)2.0)*fB0) +
             fT*(fA01*fS+fA11*fT+((Scalar)2.0)*fB1)+fC;
     }
 }
 else
 {
     Scalar fTmp0, fTmp1, fNumer, fDenom;

     if (fS < (Scalar)0.0)  // region 2
     {
         fTmp0 = fA01 + fB0;
         fTmp1 = fA11 + fB1;
         if (fTmp1 > fTmp0)
         {
             fNumer = fTmp1 - fTmp0;
             fDenom = fA00-2.0f*fA01+fA11;
             if (fNumer >= fDenom)
             {
                 fS = (Scalar)1.0;
                 fT = (Scalar)0.0;
                 fSqrDistance = fA00+((Scalar)2.0)*fB0+fC;
             }
             else
             {
                 fS = fNumer/fDenom;
                 fT = (Scalar)1.0 - fS;
                 fSqrDistance = fS*(fA00*fS+fA01*fT+2.0f*fB0) +
                     fT*(fA01*fS+fA11*fT+((Scalar)2.0)*fB1)+fC;
             }
         }
         else
         {
             fS = (Scalar)0.0;
             if (fTmp1 <= (Scalar)0.0)
             {
                 fT = (Scalar)1.0;
                 fSqrDistance = fA11+((Scalar)2.0)*fB1+fC;
             }
             else if (fB1 >= (Scalar)0.0)
             {
                 fT = (Scalar)0.0;
                 fSqrDistance = fC;
             }
             else
             {
                 fT = -fB1/fA11;
                 fSqrDistance = fB1*fT+fC;
             }
         }
     }
     else if (fT < (Scalar)0.0)  // region 6
     {
         fTmp0 = fA01 + fB1;
         fTmp1 = fA00 + fB0;
         if (fTmp1 > fTmp0)
         {
             fNumer = fTmp1 - fTmp0;
             fDenom = fA00-((Scalar)2.0)*fA01+fA11;
             if (fNumer >= fDenom)
             {
                 fT = (Scalar)1.0;
                 fS = (Scalar)0.0;
                 fSqrDistance = fA11+((Scalar)2.0)*fB1+fC;
             }
             else
             {
                 fT = fNumer/fDenom;
                 fS = (Scalar)1.0 - fT;
                 fSqrDistance = fS*(fA00*fS+fA01*fT+((Scalar)2.0)*fB0) +
                     fT*(fA01*fS+fA11*fT+((Scalar)2.0)*fB1)+fC;
             }
         }
         else
         {
             fT = (Scalar)0.0;
             if (fTmp1 <= (Scalar)0.0)
             {
                 fS = (Scalar)1.0;
                 fSqrDistance = fA00+((Scalar)2.0)*fB0+fC;
             }
             else if (fB0 >= (Scalar)0.0)
             {
                 fS = (Scalar)0.0;
                 fSqrDistance = fC;
             }
             else
             {
                 fS = -fB0/fA00;
                 fSqrDistance = fB0*fS+fC;
             }
         }
     }
     else  // region 1
     {
         fNumer = fA11 + fB1 - fA01 - fB0;
         if (fNumer <= (Scalar)0.0)
         {
             fS = (Scalar)0.0;
             fT = (Scalar)1.0;
             fSqrDistance = fA11+((Scalar)2.0)*fB1+fC;
         }
         else
         {
             fDenom = fA00-2.0f*fA01+fA11;
             if (fNumer >= fDenom)
             {
                 fS = (Scalar)1.0;
                 fT = (Scalar)0.0;
                 fSqrDistance = fA00+((Scalar)2.0)*fB0+fC;
             }
             else
             {
                 fS = fNumer/fDenom;
                 fT = (Scalar)1.0 - fS;
                 fSqrDistance = fS*(fA00*fS+fA01*fT+((Scalar)2.0)*fB0) +
                     fT*(fA01*fS+fA11*fT+((Scalar)2.0)*fB1)+fC;
             }
         }
     }
  }

  // account for numerical round-off error
  if (fSqrDistance < (Scalar)0.0)
  {
     fSqrDistance = (Scalar)0.0;
  }

  //additionally compute projected point
  _projectedPoint = _triangleVertexA + fS*kEdge0 + fT*kEdge1;

  // additionally return barycentric coordinates of projected point
  bc.u_ = 1-fS-fT;
  bc.v_ = fS;
  bc.w_ = fT;

  return fSqrDistance;
}



}}//namespaces

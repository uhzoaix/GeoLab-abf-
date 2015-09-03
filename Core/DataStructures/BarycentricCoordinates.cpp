#include "BarycentricCoordinates.h"

namespace GCL {



void BarycentricCoordinates::FromCartesian(const Vec3& _A, const Vec3& _B,
                                           const Vec3& _C, const Vec3& _P, const Scalar& tolerance)
{
    // Vectors to vertices
    Vec3 PA(_A - _P);
    Vec3 PB(_B - _P);
    Vec3 PC(_C - _P);

    Vec3 crossPCA(PA ^ PC);
    Vec3 crossPAB(PB ^ PA);
    Vec3 crossABC((_C-_A) ^ (_B-_A));

    Scalar areaABC = crossABC.length();
    Scalar areaPCA = crossPCA.length();
    Scalar areaPAB = crossPAB.length();

    // test for too small triangles
    if (areaABC > DBL_EPSILON)
    {
        v_ = areaPCA / areaABC * (((crossPCA*crossABC) > 0.0) ? 1.0 : (-1.0));
        w_ = areaPAB / areaABC * (((crossPAB*crossABC) > 0.0) ? 1.0 : (-1.0));
#ifdef DEBUG_BARYCENTRIC_COORDINATES
        assert(w_ < 1.0 + tolerance);
        assert(w_ > -tolerance);
        assert(v_ < 1.0 + tolerance);
        assert(v_ > -tolerance);
        assert(w_ + v_ < 1.0 + 2.0*tolerance);
        assert(w_ + v_ > -2.0*tolerance);
#endif

        // snap
        if (fabs(w_) < tolerance) //Changed by Mario, 11.04.2011: Uses fabs instead of abs, which casts to int, nasty!
        {
            w_ = 0.0; //Changed by Mario, 11.04.2011. Before: u_ = 0.0; Seems wrong as u_ is computed later in computeU
        }
        if (fabs(v_) < tolerance) //Changed by Mario, 11.04.2011: Uses fabs instead of abs, which casts to int, nasty!
        {
            v_ = 0.0;
        }

        if (fabs(1.0-w_) < tolerance) //Changed by Mario, 11.04.2011: Uses fabs instead of abs, which casts to int, nasty!
        {
            w_ = 1.0; //Changed by Mario, 11.04.2011. Before: u_ = 1.0; Seems wrong as u_ is computed later in computeU
        }
        if (fabs(1.0-v_) < tolerance) //Changed by Mario, 11.04.2011: Uses fabs instead of abs, which casts to int, nasty!
        {
            v_ = 1.0;
        }
        computeU();

        // snap U //Added by Mario, 11.04.2011:
        if (fabs(u_) < tolerance)
            u_ = 0.0;

        if (fabs(1.0-u_) < tolerance)
            u_ = 1.0;
    }
    else
    {
        w_ = -1.0f;
        v_ = -1.0f;
        u_ = -1.0f;
    }
}

void BarycentricCoordinates::FromCartesian(const Vec3& _A, const Vec3& _B,
                                           const Vec3& _C, const Vec3& _P)
{
    // Vectors to the vertices
    Vec3 PA(_A-_P);
    Vec3 PB(_B-_P);
    Vec3 PC(_C-_P);

    // areas
    Vec3 crossPCA(PA ^ PC);
    Vec3 crossPAB(PB ^ PA);
    Vec3 crossABC((_C-_A)^(_B-_A));

    Scalar areaABC = crossABC.length();
    Scalar areaPCA = crossPCA.length();
    Scalar areaPAB = crossPAB.length();

    // test for too small triangles
    if (areaABC < DBL_EPSILON)
    {
        u_ = DBL_MAX;
        v_ = DBL_MAX;
        w_ = DBL_MAX;
    }
    else
    {
        // fix the sign problem
        v_ = areaPCA / areaABC * (((crossPCA*crossABC) > 0.0) ? 1.0 : (-1.0));
        w_ = areaPAB / areaABC * (((crossPAB*crossABC) > 0.0) ? 1.0 : (-1.0));
        computeU();
    }
}

void BarycentricCoordinates::FromCartesian( const std::shared_ptr<OMesh> mesh, OMesh::FaceHandle fh, Vec3 p, const Scalar tolerance/*=0.00001*/){
    OMesh::ConstFaceVertexIter fvI=mesh->fv_iter(fh);
    OpenMesh::VertexHandle v0 = fvI.handle(); ++fvI;
    OpenMesh::VertexHandle v1 = fvI.handle(); ++fvI;
    OpenMesh::VertexHandle v2 = fvI.handle();
    return FromCartesian(mesh->point(v0),mesh->point(v1),mesh->point(v2),p,tolerance);
}

Vec3 BarycentricCoordinates::ToCartesian(const Vec3& _A, const Vec3& _B, const Vec3& _C) const
{
    return(ToCartesian(_A,_B,_C,u_,v_));
}


Vec3 BarycentricCoordinates::ToCartesian(const Vec3& _A, const Vec3& _B, const Vec3& _C, Vec3& _result) const
{
    return(ToCartesian(_A,_B,_C,u_,v_,_result));
}


Vec3 BarycentricCoordinates::ToCartesian(const Vec3& _A, const Vec3& _B, const Vec3& _C,
                                            const Scalar& _u, const Scalar& _v) const
{
    Q_UNUSED(_u)
    Q_UNUSED(_v)
    return(_A*u_ + _B*v_ + _C*w_);
}


Vec3 BarycentricCoordinates::ToCartesian(const Vec3& _A, const Vec3& _B, const Vec3& _C,
                                            const Scalar& _u, const Scalar& _v, Vec3& _result) const
{
    _result = ToCartesian(_A,_B,_C,_u,_v);
    return(_result);
}

Vec3 BarycentricCoordinates::ToCartesian(const std::shared_ptr<OMesh> mesh, OMesh::FaceHandle fh ){
    OMesh::ConstFaceVertexIter fvI=mesh->fv_iter(fh);
    Vec3 A=mesh->point(fvI.handle());
    Vec3 B=mesh->point((++fvI).handle());
    Vec3 C=mesh->point((++fvI).handle());
    return ToCartesian(A,B,C);
}

bool BarycentricCoordinates::isValid( Scalar tolerance/*=0.*/) const{
    bool bu= (u_>= -tolerance && u_<=1.+ tolerance);
    bool bv= (v_>= -tolerance && v_<=1.+ tolerance);
    bool bw= (w_>= -tolerance && w_<=1.+ tolerance);
    return bu && bv && bw;
}

bool BarycentricCoordinates::isVertex( Scalar tolerance/*=0.*/ ) const{
    return isValid(tolerance) && ( fabs(u_-1.)<tolerance || fabs(v_-1.)<tolerance  || fabs(w_-1.)<tolerance );
}

bool BarycentricCoordinates::isEdge( Scalar tolerance/*=0.*/ ) const{
    return isValid(tolerance) && ( u_<tolerance || v_<tolerance  || w_<tolerance );
}

OMesh::VertexHandle BarycentricCoordinates::getVertex( const std::shared_ptr<OMesh> mesh, OMesh::FaceHandle fh, Scalar tolerance/*=0.*/ ) const{

    OMesh::FaceHalfedgeIter fhI=mesh->fh_iter(fh);
    if(fabs(u_-1.)<tolerance) return mesh->to_vertex_handle(fhI.handle());
    if(fabs(v_-1.)<tolerance) return mesh->to_vertex_handle((++fhI).handle());
    if(fabs(w_-1.)<tolerance) return mesh->to_vertex_handle((++(++fhI)).handle());

    return OMesh::InvalidVertexHandle;
}

OMesh::HalfedgeHandle BarycentricCoordinates::getHalfedge( const std::shared_ptr<OMesh> mesh, OMesh::FaceHandle fh, Scalar& t, Scalar tolerance/*=0.*/ ) const{

    OMesh::FaceHalfedgeIter fhI=mesh->fh_iter(fh);
    if( w_>tolerance && u_>tolerance &&  v_<=tolerance ){ t=u_; return fhI.handle();         }
    if( u_>tolerance && v_>tolerance &&  w_<=tolerance ){ t=v_; return (++fhI).handle();     }
    if( v_>tolerance && w_>tolerance &&  u_<=tolerance ){ t=w_; return (++(++fhI)).handle(); }

    return OMesh::InvalidHalfedgeHandle;
}
}

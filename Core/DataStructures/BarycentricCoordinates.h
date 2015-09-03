#ifndef BARYCENTERICCOORDINATES_H
#define BARYCENTERICCOORDINATES_H
#include <memory>
#include <Core/Math/MathDefines.h>
#include <Core/DataStructures/OMesh.h>
namespace GCL {
/**
 * @brief  The BarycentricCoordinates class
 * @author Shiwei
 * @ref  Dolphin from ETH Zrich
 * @date 2014.11.26
 */
class GCL_CORE_SHARED_EXPORT BarycentricCoordinates
{
public:


    //! @name Constructor / destructor
    /*******************************************************************************/
    //@{
    //! Standard-constructor
    explicit BarycentricCoordinates(void) : u_(-1.0), v_(-1.0), w_(-1.0) {}
    //! Constructor taking two barycentric coordinates
    explicit BarycentricCoordinates(const Scalar& _u, const Scalar& _v) : u_(_u), v_(_v) { computeW(); }
    //! Constructor taking three barycentric coordinates
    explicit BarycentricCoordinates(const Scalar& _u, const Scalar& _v, const Scalar& _w) :  u_(_u), v_(_v), w_(_w) {}
    //! Constructor taking three points and the resulting point
    explicit BarycentricCoordinates(const Vec3& _A, const Vec3& _B,
                                    const Vec3& _C, const Vec3& _P, Scalar _tolerance = 0.00001) { FromCartesian(_A, _B, _C, _P, _tolerance); }
    explicit BarycentricCoordinates(const Vec2& _A, const Vec2& _B,
                                    const Vec2& _C, const Vec2& _P,
                                    Scalar _tolerance = 0.00001) { FromCartesian(Vec3(_A[0],_A[1],1.), Vec3(_B[0],_B[1],1.), Vec3(_C[0],_C[1],1.), Vec3(_P[0],_P[1],1.), _tolerance); }
    //! Destructor
    virtual ~BarycentricCoordinates(void) {}
    //@}

    //! @name Methods
    /*******************************************************************************/
    //@{
    void FromCartesian(const Vec3& _A, const Vec3& _B,
                       const Vec3& _C, const Vec3& _P, const Scalar& tolerance);
    void FromCartesian(const Vec3& _A, const Vec3& _B,
                       const Vec3& _C, const Vec3& _P);
    void FromCartesian( const std::shared_ptr<OMesh> mesh, OMesh::FaceHandle fh, Vec3 p, const Scalar tolerance=0.00001);

    Vec3 ToCartesian(const Vec3& _A, const Vec3& _B, const Vec3& _C) const;
    Vec3 ToCartesian(const Vec3& _A, const Vec3& _B, const Vec3& _C, const Scalar& _u, const Scalar& _v) const;
    Vec3 ToCartesian(const Vec3& _A, const Vec3& _B, const Vec3& _C, Vec3& _result) const;
    Vec3 ToCartesian(const Vec3& _A, const Vec3& _B, const Vec3& _C, const Scalar& _u, const Scalar& _v, Vec3& _result) const;
    Vec3 ToCartesian(const std::shared_ptr<OMesh> mesh, OMesh::FaceHandle fh );

    /** Checks if all of the coordinates c: -tolerance <= c <= 1.+tolerance
    \param tolerance A positive threshold on the element-wise distance to a triangle, in coordinate space
*/
    bool isValid( Scalar tolerance=0. ) const;

    /** Checks if the coordinates are less than tolerance away from a vertex
    \param tolerance A positive threshold on the element-wise distance to a vertex, in coordinate space
*/
    bool isVertex( Scalar tolerance=0. ) const;

    /** Checks if the coordinates are less than tolerance away from any edge in the triangle
    \param tolerance A positive threshold on the element-wise distance to any edge of the triangle, in coordinate space
*/
    bool isEdge( Scalar tolerance=0. ) const;

    /** Get the vertex this barycentric coordinate represents in face fh up to tolerance
    \param tolerance A positive threshold on the element-wise distance to a vertex, in coordinate space
*/
    OMesh::VertexHandle getVertex( const std::shared_ptr<OMesh> mesh, OMesh::FaceHandle fh, Scalar tolerance=0. ) const;


    /** Get the edge this barycentric coordinate represents in face fh up to tolerance
    \param tolerance  A positive threshold on the element-wise distance to a vertex, in coordinate space
    \param t          The position of the barycentric coordinate on the edge, where t=0 at begining of the Halfedge, t=1 at the end
*/
    OMesh::HalfedgeHandle getHalfedge( const std::shared_ptr<OMesh> mesh, OMesh::FaceHandle fh,  Scalar& t,  Scalar tolerance=0. ) const;

    //@}

private:
    //! computes u-coordinate
    inline void computeU(void) { u_ = 1.0 - w_ - v_; }
    //! computes u-coordinate
    inline void computeV(void) { v_ = 1.0 - u_ - w_; }
    //! computes w-coordinate
    inline void computeW(void) { w_ = 1.0 - u_ - v_; }

public:
    Scalar u_;		//!< first barycentric coordinate
    Scalar v_;		//!< second barycentric coordinate
    Scalar w_;		//!< third barycentric coordinate

    Scalar& operator[](int i){
        if(i==0){
            return u_;
        }else if(i==1){
            return v_;
        }else if(i==2){
            return w_;
        }

        //check bounds, and return value against compiler warning
        assert(false);
        return u_;
    }

};
}
#endif // BARYCENTERICCOORDINATES_H

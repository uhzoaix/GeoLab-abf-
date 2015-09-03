#ifndef GCL_FIBERMESHFAIRING_H
#define GCL_FIBERMESHFAIRING_H
#include <Core/Math/MathDefines.h>
#include "Utilities/OpenMeshAlgorithms/gcl_export.h"
#include "FairingBase.h"
#include <memory>
#include <vector>
namespace GCL {
namespace Utilities {
/**
 * @brief The FiberMeshFairing class
 * This class implements the mesh optimization algorithm in the paper [1].
 * There are many objective functions for fairing a mesh, each of it has its own strengthes and weaknesses.
 * The paper [1] select the following objective function:
 *              min || L * H  ||^2
 * where L is the discrete Laplace-Beltrami operator, and H = (k_1 + k_2)/2 is the mean curvature.
 * Denote c_i as mean curvature value on each vertex, v_i as position of each vertex;
 * There are two optimization step in each iteration:
 * In first step:
 *          min || L * c ||^2 + ||c - c' ||^2
 * Then let b_i = c_i * n_i;
 *
 * The second step:
 *          min || L * v - b ||^2  + || v - v' || ^ 2
 *
 * @ref [1] Andrew Nealen, Takeo Igarashi, e.d. 2006 FiberMesh: Design Freeform Surfaces with 3D Curves.
 * @author Shiwei
 * @date 2014.12.8
 */
class LeastSquareLaplacian;
class GCL_OPENMESHALGORITHMS_SHARED_EXPORT FiberMeshFairing : public FairingBase
{
public:
    FiberMeshFairing();

    bool fairing(OMesh *mesh);
private:
    /**
     * @brief initLaplacianProblem
     * @return
     */
    bool initLaplacianProblem();

    /**
     * @brief computeMeanCurvaturePerVertex
     * c_i = || L * v_i ||
     */
    void computeMeanCurvaturePerVertex();

    /**
     * @brief solveLaplacianCurvatureEnerge     *
     * min ||c||^2 + || c - c'||^2
     * @return
     */
    int solveLaplacianCurvatureEnerge();

    /**
     * @brief computeLaplacianCoordinatesPerVertex
     * Lv_i = c_i * n_i
     */
    void computeLaplacianCoordinatesPerVertex();

    /**
     * @brief solveLaplacianPositionEnerge
     * @return
     */
    bool solveLaplacianPositionEnerge();


    std::vector< Scalar > c_;   //*< mean curvature per vertex;
    std::vector< Vec3 > Lv_;  //*< Laplacian coordinates per vertex;
    OMesh* mesh_;
    bool onlyOnSelectedVertices_;
    Scalar lambda_;
    int iter_Num_;
    std::shared_ptr<LeastSquareLaplacian > ls_laplacian_solver_;

    Scalar tolerance_;
};
}}
#endif // FIBERMESHFAIRING_H

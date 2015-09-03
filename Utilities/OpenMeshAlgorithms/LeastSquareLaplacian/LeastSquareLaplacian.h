#ifndef LEASTSQUARELAPLACIAN_H
#define LEASTSQUARELAPLACIAN_H
#include <Utilities/OpenMeshAlgorithms/gcl_export.h>
#include <vector>
#include <map>
#include <Core/Math/MathDefines.h>
#include <Utilities/OpenMeshAlgorithms/gcl_export.h>

namespace GCL {
class OMesh;
namespace Utilities {
/**
 * @brief The LeastSquareLaplacian class
 * This class is used to solve the leastsquare Laplacian problem on OpenMesh,
 * which has the form like this:
 *      ||Lx - b||^2 + \lambda * ||x - x'||^2
 * @author Shiwei
 * @date 2014.12.4
 */
class GCL_OPENMESHALGORITHMS_SHARED_EXPORT LeastSquareLaplacian
{
public:
    typedef std::vector< std::map<int, Scalar> > RowMaps;
    LeastSquareLaplacian();

    virtual ~LeastSquareLaplacian() {}

    /**
     * @brief compute
     * @param mesh
     * @param _lambda
     * @param contraints_on_selected_point_only
     * @return
     */
    virtual bool compute(const OMesh& mesh, Scalar _lambda = 1.0, bool constraints_on_selected_point_only = false, bool is_hard_constraints = true);

    /**
     * @brief solve
     *      solve the problem
     * @param lb  the laplacian problem ||Lx - lb||^2
     * @param b the contraints on x: ||x - b|| in some set
     * @param x the solution
     * @return sucessful or not
     */
    virtual bool solve(const std::vector<Scalar>& lb, const std::vector<Scalar>& b, std::vector<Scalar>& x) = 0;

    bool solve(const std::vector<Vec3> & lbv, const std::vector<Vec3>& bv, std::vector<Vec3>& xv)
    {
        std::vector<Scalar> lb[3];
        std::vector<Scalar> b[3];
        std::vector<Scalar> x[3];
        for(int j=0; j < 3; j++)
        {
            for(size_t i=0; i < lbv.size(); i++)
            {
                lb[j].push_back(lbv[i][j]);
            }
            for(size_t i=0; i < bv.size(); i++)
            {
                b[j].push_back(bv[i][j]);
            }
            if(!solve(lb[j],b[j],x[j])) return false;
        }
        xv.clear();
        for(size_t i=0; i < x[0].size(); i++)
        {
           Vec3 v(x[0][i],x[1][i],x[2][i]);
           xv.push_back(v);
        }
        return true;
    }

protected:
    virtual bool compute_matrices(const OMesh& mesh) = 0;
    void initProblemSize(const OMesh& mesh);
    void initLaplaceMatrix(const OMesh& mesh, RowMaps& Lmap);
    void initContraintsMatrix(const OMesh& mesh, RowMaps& Imap);


    bool preprocess_LaplacianVector(std::vector<Scalar>& lb,const std::vector<Scalar>& b);
    bool preprocess_ConstraintsVector(std::vector<Scalar>& b);
    bool postprocess_ResultVector( std::vector<Scalar>& x);

    Scalar lambda_;
    int n_L_rows_;
    int n_I_rows_;
    int n_cols_;
    bool constraints_on_selected_point_only_;
    bool is_hard_constraints_;
    std::vector<int> v_index_;
    std::vector<Scalar> b_constraints_;
    const OMesh* mesh_;
};
}}
#endif // LEASTSQUARELAPLACIAN_H

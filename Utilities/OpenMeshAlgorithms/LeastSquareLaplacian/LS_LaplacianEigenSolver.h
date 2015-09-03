#ifndef LS_LAPLACIANEIGENSOLVER_H
#define LS_LAPLACIANEIGENSOLVER_H
#include "LeastSquareLaplacian.h"

#include <Eigen/Sparse>
namespace GCL {
namespace Utilities {

template <typename SparseSolver >
class LS_LaplacianEigenSolver : public LeastSquareLaplacian
{
public:
    typedef Eigen::SparseMatrix<Scalar> MatrixType;
    LS_LaplacianEigenSolver() {}

    bool compute_matrices(const OMesh& mesh)
    {
        if(n_I_rows_ == 0)
        {
            DebugLog<<"Error: n_I_rows == 0"<<std::endl;
            return false;
        }
        lambda_ = lambda_ * (Scalar)n_L_rows_ / (Scalar)n_I_rows_;
        std::vector< std::map<int, Scalar> > Lmap;
        std::vector< std::map<int, Scalar> > Imap;
        initLaplaceMatrix(mesh,Lmap);
        setMatrixFromRowMaps(Lmap,L_);
        initContraintsMatrix(mesh,Imap);
        if(Imap.size() == 0)
        {
            solver_.compute(L_.transpose() * L_ );
        }
        else
        {
            setMatrixFromRowMaps(Imap,I_);
            solver_.compute(L_.transpose() * L_ + lambda_ * I_.transpose() * I_);
        }
        if(solver_.info() == Eigen::Success)
        {
            DebugLog<<"Succesfull"<<std::endl;
            return true;
        }
        return false;
    }
    bool solve(const std::vector<Scalar> &_lb, const std::vector<Scalar> &_b, std::vector<Scalar> &x)
    {
        std::vector<Scalar> lb(_lb);
        std::vector<Scalar> b(_b);

        if(!preprocess_LaplacianVector(lb,b)) return false;
        if(!preprocess_ConstraintsVector(b)) return false;
        Eigen::VectorXd lb0(lb.size());
        for(size_t i=0; i < lb.size(); i++)
        {
            lb0.coeffRef(i) = lb[i];
        }
        Eigen::VectorXd bb = L_.transpose() * lb0;
        if(b.size() > 0)
        {
            Eigen::VectorXd b0(b.size());
            for(size_t i=0; i < b.size(); i++)
            {
                b0.coeffRef(i) = b[i];
            }
            Eigen::VectorXd Itb = I_.transpose() * b0;
            bb = bb + lambda_ * Itb;
        }
        Eigen::VectorXd ansx = solver_.solve(bb);

        if(solver_.info() == Eigen::Success)
        {
            DebugLog<<"Succeful Solved"<<std::endl;
            x.clear();
            for(int i=0; i < n_cols_; i++)
            {
                x.push_back(ansx.coeff(i));
            }

            return postprocess_ResultVector(x);
        }
        else
        {
            DebugLog<<"Failed~!"<<std::endl;
        }

        return false;
    }

private:
    void setMatrixFromRowMaps(const RowMaps& rmap, MatrixType& mat)
    {
        mat.resize(rmap.size(),n_cols_);
        std::vector< Eigen::Triplet<Scalar> > triplets;
        for(size_t i=0; i < rmap.size(); i++)
        {
            for(std::map<int,Scalar>::const_iterator itr= rmap[i].begin(); itr != rmap[i].end(); itr++)
            {
                int j = itr->first;
                Scalar value = itr->second;
                triplets.push_back(Eigen::Triplet<Scalar>(i,j,value));
            }
        }
        mat.setFromTriplets(triplets.begin(),triplets.end());
    }

    MatrixType L_;
    MatrixType I_;
    SparseSolver solver_;

};
}}
#endif // LS_LAPLACIANEIGENSOLVER_H

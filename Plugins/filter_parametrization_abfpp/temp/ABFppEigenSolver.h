#pragma once
#include "ABFpp.h"
#include <Eigen/Core>
#include <Eigen/SparseLU>
//#include <Eigen\SuperLUSupport>

namespace GCL {
namespace Utilities{

// to support Other Eigen solver, but recommend superLU

class ABFppEigenSolver :
	public ABFpp
{
public:

	typedef Eigen::SparseMatrix<double> MatrixType;
	//typedef Eigen::SuperLU <MatrixType> SuperLUSolver;
	typedef Eigen::SparseLU<Eigen::SparseMatrix<double> >SparseLUSolver;

	typedef Eigen::Triplet<double> Tl;
	ABFppEigenSolver();
	~ABFppEigenSolver();

public:
	// compute b1,b2,J1,J2,A,A inverse, refer to paper (6),(11)
	void ComputeMatrixA(MatrixType& Ainv,MatrixType& A);
	void ComputeMatrixB(MatrixType& b1,MatrixType& b2_tri,MatrixType& b2_pl);
	void ComputeMatrixJ(MatrixType& J1,MatrixType& J2);
	// compute rhs and lhs matrix by b1,b2,J1,J2,A
	void ComputeMatrix(Eigen::VectorXd& RHS_mat, MatrixType& LHS_mat);

	// to solve linear equation using Eigen Solver
	// delta is the solution of equation, refer to the paper , (6)
	void Solve(Eigen::VectorXd& RHS_mat, MatrixType& LHS_mat, Eigen::VectorXd& delta_lambda2);

	// add delta to existing alpha and lambda
	// use for Newton iterative method
	void UpdateAlphaAndLambda(Eigen::VectorXd& delta_lambda2); 

public:
	// Assisting function
	// get angle's index giving face index and vertex index
	int GetAngleIdx(int f_idx,int v_idx);
	// merge two matrices which have the same columns
	void MergeMatrix(MatrixType& top_mat,MatrixType& bot_mat,MatrixType& merged_mat);
	// to solve the inverse of A_ask
	void DiagnalInverse(MatrixType& A_ask,MatrixType& A_ask_inv);

	// for debug
	void ObservMatrix(Eigen::MatrixXd& mat);
	void ObservMatrix(MatrixType& mat);
	void ObservVector(Eigen::VectorXd& vec);
private:
	MatrixType J1_;
	MatrixType J2_;
	MatrixType b1_;
	MatrixType b2_tri_;

	SparseLUSolver solver_;
};

}
}


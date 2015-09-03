#include "ABFppEigenSolver.h"
#include <Core/DataStructures/OMesh.h>
#include <iostream>

namespace GCL{ namespace Utilities{

ABFppEigenSolver::ABFppEigenSolver()
{
}


ABFppEigenSolver::~ABFppEigenSolver()
{
}

void ABFppEigenSolver::ComputeMatrix(Eigen::VectorXd& RHS_mat,MatrixType& LHS_mat)
{
	int num_face = beta_triangles_.size();
	int num_angle = alpha_.size();

	MatrixType b1	 (num_angle, 1);
	MatrixType b2_tri(num_face,1);
	MatrixType b2_pl (2 * num_interior_, 1); 
	
	MatrixType J1(num_face, num_angle);
	MatrixType J2(2 * num_interior_, num_angle);
	
	MatrixType Ainv(num_angle, num_angle); 
	MatrixType A   (num_angle, num_angle);						// b2_tri is Nf *1,b2_pl is (2*Nint) * 1

	ComputeMatrixA(Ainv,A);
	ComputeMatrixB(b1,b2_tri,b2_pl);
	ComputeMatrixJ(J1,J2);

	MatrixType b1_ask(num_face, 1);
	MatrixType b2_ask(2 * num_interior_,1);
	MatrixType A_ask(num_face,num_face);
	MatrixType A_ask_inv(num_face,num_face); 
	MatrixType J_ask(2 * num_interior_, num_face);
	MatrixType J_2ask(2 * num_interior_, 2 * num_interior_);				// e.g. matrix b asterik,refer to (12)
	
	b1_ask = J1 * Ainv * b1 - b2_tri;
	b2_ask = J2 * Ainv * b1 - b2_pl;
	A_ask = J1 * Ainv * J1.transpose();
	DiagnalInverse(A_ask , A_ask_inv);
	J_ask  = J2 * Ainv * J1.transpose();
	J_2ask = J2 * Ainv * J2.transpose();

	LHS_mat = J_ask * A_ask_inv * J_ask.transpose() - J_2ask;
	RHS_mat = J_ask * A_ask_inv * b1_ask - b2_ask;

	// Save mat for Update Alpha and Lambda Step
	J2_ = J2;
	J1_ = J1;
	b1_ = b1;
	b2_tri_ = b2_tri;		// aim to calculate b1_ask
}

void ABFppEigenSolver::Solve(Eigen::VectorXd& RHS_mat, MatrixType& LHS_mat,Eigen::VectorXd& delta_lambda2)
{
	solver_.compute(LHS_mat); 
	if (solver_.info() != Eigen::Success)
	{
		// Decomposition failed
		std::cout << "Failed to decompose matrix" << std::endl;
		return;
	}

	delta_lambda2 = solver_.solve(RHS_mat);

	if (solver_.info() != Eigen::Success)
	{
		// solve failed
		std::cout << "Failed to solve" << std::endl;
		return;
	}
	
}

void ABFppEigenSolver::ComputeMatrixA(MatrixType& Ainv, MatrixType& A)
{
	// TODO: found A and Ainv in Initial step, and save locally
	std::vector<Tl> tripletList;
	std::vector<Tl> tripletList_inv;
	double temp_angle = 0;
	for (int t = 0;t < beta_triangles_.size();++t)
	{
		for (int k = 0;k < 3;++k)
		{
			temp_angle = beta_triangles_[t].angles_[k];
			
			tripletList.push_back(Tl(3 * t + k, 3 * t + k,
				2.0/(temp_angle * temp_angle) ));					// pushback beta(t,k) as a diagnal element

			tripletList_inv.push_back(Tl(3 * t + k, 3 * t + k, 
				(temp_angle * temp_angle)/2.0));
		}
	}

	A.setFromTriplets(tripletList.begin(),tripletList.end());
	Ainv.setFromTriplets(tripletList_inv.begin(),tripletList_inv.end());
}

void ABFppEigenSolver::ComputeMatrixB(MatrixType& b1, MatrixType& b2_tri, MatrixType& b2_pl)
{
	std::vector <Tl> tripletList_b1;
	std::vector <Tl> tripletList_tri;
	std::vector <Tl> tripletList_pl;

	// found matrix b1
	for (int t = 0;t < beta_triangles_.size();++t)
	{
		for (int k = 0;k < 3;++k)
		{
			// pushback the minus partial alpha(t,k)
			tripletList_b1.push_back(Tl(3*t+k,0, - CalculateAlphaPara(t,k)));
		}
	}
	b1.setFromTriplets(tripletList_b1.begin(),tripletList_b1.end());

	// found matrix b2_tri
	for (int t = 0;t < beta_triangles_.size();++t)
	{
		tripletList_tri.push_back(Tl(t,0, - CalculateTriPara(t)));
	}
	b2_tri.setFromTriplets(tripletList_tri.begin(),tripletList_tri.end());

	//found matrix b2_pl
	int pos = 0;
	for (int i = 0;i < mesh_->n_vertices_();++i)
	{
		if (! mesh_->is_boundary(mesh_->vertex_handle(i)))				// if v(i) isn't a boundary point
		{
			tripletList_pl.push_back(Tl(pos,0, - CalculatePlanPara(i)));
			pos++;
		}
	}
	for (int i = 0; i < mesh_->n_vertices_(); ++i)
	{
		if (!mesh_->is_boundary(mesh_->vertex_handle(i)))
		{
			tripletList_pl.push_back(Tl(pos, 0, - CalculateLenPara(i)));
			pos++;
		}
	}
	b2_pl.setFromTriplets(tripletList_pl.begin(),tripletList_pl.end());
}

void ABFppEigenSolver::ComputeMatrixJ(MatrixType& J1, MatrixType& J2)
{
	std::vector <Tl> tripletList_J1;
	std::vector <Tl> tripletList_J2;

	// Found J1
	for (int i = 0;i < beta_triangles_.size();++i)
	{
		tripletList_J1.push_back(Tl(i, 3 * i,1));
		tripletList_J1.push_back(Tl(i, 3 * i + 1, 1));
		tripletList_J1.push_back(Tl(i, 3 * i + 2, 1));
	}
	J1.setFromTriplets(tripletList_J1.begin(),tripletList_J1.end());

	// Found J2
	int pos = 0;
	
	// parameter plan part
	int temp_f_idx = 0;
	int temp_v_idx = 0;
	int temp_angle_idx = 0;

	for (int i = 0; i < mesh_->n_vertices_(); ++i)
	{
		if (!mesh_->is_boundary(mesh_->vertex_handle(i)))
		{
			OMesh::HalfedgeHandle he_it0 = mesh_->halfedge_handle(mesh_->vertex_handle(i));
			// run over halfedges around v(i)
			for (OMesh::HalfedgeHandle he_it = he_it0; ;)
			{
				he_it = mesh_->opposite_halfedge_handle(he_it);

				temp_f_idx = mesh_->face_handle(he_it).idx();
				temp_v_idx = i;

				temp_angle_idx = GetAngleIdx(temp_f_idx,temp_v_idx);

				tripletList_J2.push_back(Tl(pos,temp_angle_idx,1));

				he_it = mesh_->next_halfedge_handle(he_it);

				if (he_it == he_it0) break;
			}
			pos++;
		}
	}

	//parameter len part
	std::vector<int> adj_angle_list1;						// an angle index list adjacent to v(i)
	std::vector<int> adj_angle_list2;						// another angle index list whose member is the third angle in triangle
	std::vector<double> sin_alpha_list1;
	std::vector<double> sin_alpha_list2;
	std::vector<double> alpha_list1;
	std::vector<double> alpha_list2;

	double temp_product1 = 1;
	double temp_product2 = 1;
	double cot1 = 0;
	double cot2 = 0;

	for (int i = 0; i < mesh_->n_vertices_(); ++i)
	{
		if (!mesh_->is_boundary(mesh_->vertex_handle(i)))
		{
			temp_product1 = 1;
			temp_product2 = 1;
			adj_angle_list1.clear();
			adj_angle_list2.clear();
			sin_alpha_list1.clear();
			sin_alpha_list2.clear();
			alpha_list1.clear();
			alpha_list2.clear();

			// get the outgoing halfedge and incoming halfedge
			OMesh::HalfedgeHandle og_he = mesh_->halfedge_handle(mesh_->vertex_handle(i));
			OMesh::HalfedgeHandle ig_he = mesh_->opposite_halfedge_handle(og_he);

			for (OMesh::HalfedgeHandle he_it = og_he; ;)
			{
				og_he = mesh_->next_halfedge_handle(ig_he);
				// get one vertex's angle
				temp_f_idx = mesh_->face_handle(og_he).idx();
				temp_v_idx = mesh_->to_vertex_handle(og_he).idx();				
				
				adj_angle_list2.push_back(GetAngleIdx(temp_f_idx,temp_v_idx));
				alpha_list2.push_back(GetAngleAlpha(temp_f_idx,temp_v_idx));
				sin_alpha_list2.push_back(sin(GetAngleAlpha(temp_f_idx,temp_v_idx)));
				// get another vertex's angle
				temp_v_idx = mesh_->from_vertex_handle(ig_he).idx();
				
				adj_angle_list1.push_back(GetAngleIdx(temp_f_idx,temp_v_idx));
				alpha_list1.push_back(GetAngleAlpha(temp_f_idx, temp_v_idx));
				sin_alpha_list1.push_back(sin(GetAngleAlpha(temp_f_idx, temp_v_idx)));

				ig_he = mesh_->opposite_halfedge_handle(og_he);
				// visit a round thus break
				if (he_it == og_he) break;	
			}

			// calculte all the sin product 
			for (int i = 0;i < adj_angle_list1.size();++i)
			{
				temp_product1 *= sin_alpha_list1[i];
				temp_product2 *= sin_alpha_list2[i];
			}

			for (int i = 0;i < adj_angle_list1.size();++i)
			{
				cot1 = 1.0 / tan(alpha_list1[i]);
				cot2 = 1.0 / tan(alpha_list2[i]);

				cot1 *= temp_product1;
				cot2 *= temp_product2;

				tripletList_J2.push_back(Tl(pos, adj_angle_list1[i], -cot1));	//	for debug: -cot1 instead of -cot2
				tripletList_J2.push_back(Tl(pos, adj_angle_list2[i], cot2));
			}

			pos++;
		}
	}

	J2.setFromTriplets(tripletList_J2.begin(), tripletList_J2.end());
}

void ABFppEigenSolver::UpdateAlphaAndLambda(Eigen::VectorXd& delta_lambda2)
{
	// calculate delta lambda1.delta alpha,refer to (17) and (10)
	int num_face  = beta_triangles_.size();
	int num_angle = alpha_.size();
	Eigen::MatrixXd delta_lambda1(num_face,1);
	Eigen::MatrixXd delta_alpha(num_angle,1);
	Eigen::MatrixXd delta_lambda(num_face + 2 * num_interior_,1);

	MatrixType A(num_angle,num_angle);
	MatrixType A_inv(num_angle,num_angle);
	MatrixType A_ask(num_face,num_face);
	MatrixType A_ask_inv(num_face, num_face);
	MatrixType J_ask(2 * num_interior_,num_face);
	MatrixType J(num_face + 2 * num_interior_,num_angle);
	Eigen::MatrixXd b1_ask(num_face,1);

	ComputeMatrixA(A_inv,A);
	A_ask = J1_ * A_inv * J1_.transpose();
	DiagnalInverse(A_ask,A_ask_inv);
	b1_ask = J1_ * A_inv * b1_ - b2_tri_;
	J_ask = J2_ * A_inv * J1_.transpose();

	// merge J1 and J2 to J
	MergeMatrix(J1_,J2_,J);

	// calc delta lambda1 and delta lambda and delta alpha
	delta_lambda1 = A_ask_inv * (b1_ask - J_ask.transpose() * delta_lambda2);

	for (int i = 0; i < num_face; ++i)
	{
		delta_lambda(i, 0) = delta_lambda1(i,0);
	}
	for (int i = 0; i < 2 * num_interior_;++i)
	{
		delta_lambda(i + num_face, 0) = delta_lambda2(i,0);
	}

	delta_alpha = A_inv * (b1_.toDense() - J.transpose() * delta_lambda);

	// Update step
	int pos = 0;

	for (size_t i = 0; i < alpha_.size(); ++i)
	{
		alpha_[i] += delta_alpha(i, 0);
	}

	for (size_t i = 0; i < lambda_tri_.size(); ++i)
	{
		lambda_tri_[i] += delta_lambda1(i, 0);
	}

	for (size_t i = 0; i < lambda_plan_.size(); ++i)
	{
		// lambda_plan & lambda_len only defines with interior point
		if (!mesh_->is_boundary(mesh_->vertex_handle(i)))
		{
			lambda_plan_[i] += delta_lambda2(pos, 0);
			pos++;
		}
	}

	for (size_t i = 0; i < lambda_len_.size(); ++i)
	{
		if (!mesh_->is_boundary(mesh_->vertex_handle(i)))
		{
			lambda_len_[i] += delta_lambda2(pos, 0);
			pos++;
		}
	}

}

// Assisting function

int ABFppEigenSolver::GetAngleIdx(int f_idx,int v_idx)
{
	for (int i = 0;i < 3;++i)
	{
		if (beta_triangles_[f_idx].index_[i] == v_idx)
		{
			return (3 * f_idx + i);
		}
	}

	return -1;
}

void ABFppEigenSolver::MergeMatrix(MatrixType& top_mat, MatrixType& bot_mat, MatrixType& merged_mat)
{
	MatrixType temp_top(top_mat.rows() + bot_mat.rows(), top_mat.rows());
	MatrixType temp_bot(top_mat.rows() + bot_mat.rows(), bot_mat.rows());

	std::vector<Tl> triplet1;
	std::vector<Tl> triplet2;

	for (int i = 0;i < top_mat.rows();++i)
	{
		triplet1.push_back(Tl(i,i,1));
	}
	temp_top.setFromTriplets(triplet1.begin(),triplet1.end());

	for (int i = 0;i < bot_mat.rows();++i)
	{
		triplet2.push_back(Tl(i + top_mat.rows(),i,1) );
	}
	temp_bot.setFromTriplets(triplet2.begin(),triplet2.end());

	merged_mat = temp_top * top_mat + temp_bot * bot_mat;

}

void ABFppEigenSolver::DiagnalInverse(MatrixType& A_ask,MatrixType& A_ask_inv)
{
	double temp_value = 0;
	std::vector <Tl> triplet;
	for (int i = 0; i < A_ask.rows();++i)
	{
		temp_value = A_ask.coeff(i,i);
		triplet.push_back(Tl(i, i, 1 / temp_value));
	}

	A_ask_inv.setFromTriplets(triplet.begin(),triplet.end());
}

// for debug...

void ABFppEigenSolver::ObservMatrix(Eigen::MatrixXd& mat)
{
	std::vector<Tl> triplet;

	for (int i = 0;i < mat.rows();++i)
	{
		for (int j = 0;j < mat.cols();++j)
		{
			triplet.push_back(Tl(i,j,mat(i,j)));
		}
	}

	return;
}

void ABFppEigenSolver::ObservMatrix(MatrixType& mat)
{
	std::vector<Tl> triplet;

	for (int i = 0; i < mat.rows(); ++i)
	{
		for (int j = 0; j < mat.cols(); ++j)
		{
			if (mat.coeff(i,j) != 0)
			{
				triplet.push_back(Tl(i,j,mat.coeff(i,j)));
			}
		}
	}

	return;
}

void ABFppEigenSolver::ObservVector(Eigen::VectorXd& vec)
{
	std::vector<double> temp;

	for (int i = 0;i < vec.rows();++i)
	{
		temp.push_back(vec(i,0));
	}

	return;
}

} }

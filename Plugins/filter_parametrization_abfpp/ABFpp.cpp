#include "ABFpp.h"
#include <Core/DataStructures/OMesh.h>
namespace GCL{ namespace Utilities{

ABFpp::ABFpp()
{
}


ABFpp::~ABFpp()
{
}

void ABFpp::InitProblem(OMesh* mesh, std::vector<MeshTriangle>& triangles,unsigned int NumInter)		// Number of interior point
{
	num_interior_ = NumInter;
	mesh_ = mesh;
	// extract angles to vector 'alpha_' as the initial value 
	for (size_t i = 0;i < triangles.size();++i)
	{
		alpha_.push_back(triangles[i].angles_[0]);
		alpha_.push_back(triangles[i].angles_[1]);
		alpha_.push_back(triangles[i].angles_[2]);

		beta_triangles_.push_back(triangles[i]);
	}
	// Init parameters to 1 as the initial value
	lambda_tri_.resize(triangles.size());
	lambda_plan_.resize(mesh->n_vertices_());
	lambda_len_.resize(mesh->n_vertices_());

	for (size_t i = 0;i < triangles.size();++i)
	{
		lambda_tri_[i] = 1;
	}

	for (size_t i = 0; i < mesh->n_vertices_(); ++i)
	{
		lambda_plan_[i] = 1;
		lambda_len_[i]  = 1;
	}
}

double ABFpp::NormGradF()
{
	//this function calculate the Norm of gradient of F(x)
	double norm = 0;
	//1st Step: partial derivatives to alhpa
	double p_alpha = 0;

	for (size_t t = 0;t < beta_triangles_.size();++t)					// Number of faces
	{
		for (int k = 0;k < 3;++k)									// Number of angles
		{
			p_alpha = CalculateAlphaPara(t,k);						// partial derv of alpha[t][k]

			norm += p_alpha * p_alpha;
		}
	}
	//2nd Step: partial derivatives to lambda_tri
	double p_lambda_tri = 0;
	for (size_t t = 0;t < beta_triangles_.size();++t)
	{
		p_lambda_tri = CalculateTriPara(t);

		norm += p_lambda_tri * p_lambda_tri;
	}

	//3rd Step: partial derivatives to lambda_plan
	double p_lambda_plan = 0;
	for (size_t i = 0;i < mesh_->n_vertices_();++i)
	{
		if (!mesh_->is_boundary_(mesh_->vertex_handle_(i)))
		{
			p_lambda_plan = CalculatePlanPara(i);

			norm += p_lambda_plan * p_lambda_plan;
		}
	}

	//4th Step: partial derivatives to lambda_len
	double p_lambda_len = 0;
	for (size_t i = 0;i < mesh_->n_vertices_();++i)
	{
		if (!mesh_->is_boundary_(mesh_->vertex_handle_(i)))
		{
			p_lambda_len = CalculateLenPara(i);

			norm += p_lambda_len * p_lambda_len;
		}
	}

	return sqrt(norm);
}

// Assisting function
double ABFpp::GetAngle(int f_idx,int v_idx)
{
	for (int i = 0;i < 3;++i)
	{
		if (beta_triangles_[f_idx].index_[i] == v_idx)
		{
			return beta_triangles_[f_idx].angles_[i];
		}
	}

	return -666;												// just for fun....,in general it will return angle if input is valid
}

double ABFpp::GetAngleAlpha(int idx)
{
	return (alpha_[idx]);
}

double ABFpp::GetAngleAlpha(int f_idx,int v_idx)
{
	for (int i = 0; i < 3; ++i)
	{
		if (beta_triangles_[f_idx].index_[i] == v_idx)
		{
			return alpha_[3 * f_idx + i];
		}
	}

	return -666;
}

bool ABFpp::CheckBoundary(int t,int k)
{
	// only work with + - 1
	if (k == -1) k = 2;
	if (k ==  3) k = 0;

	return (mesh_->is_boundary_(mesh_->vertex_handle_(beta_triangles_[t].index_[k])));
}

// partial derivation to alpha , the part of Sin(alpha)'s product
double ABFpp::CalculateAlphaLenPara(int t, int k)
{
	double result = 0;
	int temp_v_idx = 0;
	int temp_f_idx = 0;

	int k_prev = k - 1;
	int k_lat  = k + 1;
	if (k == 0) k_prev = 2;
	if (k == 2) k_lat  = 0;

	// the partial derivation of alpha(t,k) only accur in len restricts of v(t,k-1) and v(t,k+1)
	if (!CheckBoundary(t,k-1))
	{
		double product1 = 1;
		
		// get halfedge of vertex(t,k-1)
		OMesh::HalfedgeHandle hfh = mesh_->halfedge_handle(mesh_->vertex_handle(
									   beta_triangles_[t].index_[k_prev]));
		for (OMesh::HalfedgeHandle hf_itr = hfh; hf_itr.is_valid();)
		{
			// just the angle needed to be derived
			if (mesh_->face_handle(hfh).idx() == t)				// if the face is the t th face,then v(t,k) is in it
			{
				product1 *= cos(alpha_[3 * t + k]);
				
				hfh = mesh_->opposite_halfedge_handle(hfh);
				hfh = mesh_->next_halfedge_handle(hfh);

				if (hf_itr == hfh) hf_itr.reset();				// to end the circulation
				continue;
			}

			temp_f_idx = mesh_->face_handle(hfh).idx();			// refer to paper (4)
			temp_v_idx = mesh_->to_vertex_handle(hfh).idx();

			product1 *= sin(GetAngleAlpha(temp_f_idx,temp_v_idx));

			hfh = mesh_->opposite_halfedge_handle(hfh);
			hfh = mesh_->next_halfedge_handle(hfh);

			if (hf_itr == hfh) break;
		}

		result += ( product1 * lambda_len_[beta_triangles_[t].index_[k_prev]]);
	}

	
	if (!CheckBoundary(t,k+1))
	{
		double product2 = 1;
		OMesh::HalfedgeHandle hfh_2 = mesh_->halfedge_handle(mesh_->vertex_handle(
			beta_triangles_[t].index_[k_lat]));
		for (OMesh::HalfedgeHandle hf_itr = hfh_2;hf_itr.is_valid() ;)
		{
			hfh_2 = mesh_->opposite_halfedge_handle(hfh_2);

			// note now is the outgoing vertex
			if (mesh_->face_handle(hfh_2).idx() == t)
			{
				product2 *= cos(alpha_[3 * t + k]);
				// to get the next outgoing halfedge
				hfh_2 = mesh_->next_halfedge_handle(hfh_2);

				if (hf_itr == hfh_2) hf_itr.reset();
				continue;
			}

			temp_f_idx = mesh_->face_handle(hfh_2).idx();
			temp_v_idx = mesh_->from_vertex_handle(hfh_2).idx();

			product2 *= sin(GetAngleAlpha(temp_f_idx,temp_v_idx));
			
			hfh_2 = mesh_->next_halfedge_handle(hfh_2);

			if (hf_itr == hfh_2) break;
		}

		result -= (product2 * lambda_len_[beta_triangles_[t].index_[k_lat]]);
	}

	return result;
}

double ABFpp::CalculatePlanPara(int v_idx)
{
	// v_idx must be interior point
	const double pi = 3.1415926;
	double angle_sum = 0;
	OMesh::VertexFaceIter vf0 = mesh_->vf_iter_(	mesh_->vertex_handle_(v_idx));					// visit each face around the vertex(t,k)
	
	for (;vf0;++vf0)
	{
		angle_sum += GetAngleAlpha((*vf0).idx(),v_idx);
	}

	return (angle_sum - 2 * pi);
}

double ABFpp::CalculateLenPara(int v_idx)
{
	// v must be interior point
	double product1 = 1;
	double product2 = 1;
	int temp_f_idx = 0;
	int temp_v_idx = 0;

	OMesh::HalfedgeHandle og_he;				// outgoing halfedge
	OMesh::HalfedgeHandle ig_he;				// ingoing  halfedge

	og_he = mesh_->halfedge_handle(mesh_->vertex_handle(v_idx));
	ig_he = mesh_->opposite_halfedge_handle(og_he);

	for (OMesh::HalfedgeHandle he_itr = og_he; ;)
	{
		// og_he move to next outgoing edge
		og_he = mesh_->next_halfedge_handle(ig_he);

		temp_f_idx = mesh_->face_handle(ig_he).idx();
		temp_v_idx = mesh_->from_vertex_handle(ig_he).idx();
		product1 *= sin(GetAngleAlpha(temp_f_idx,temp_v_idx));

		temp_v_idx = mesh_->to_vertex_handle(og_he).idx();
		product2 *= sin(GetAngleAlpha(temp_f_idx,temp_v_idx));

		// ig_he move to next incoming edge
		ig_he = mesh_->opposite_halfedge_handle(og_he);

		// quit the circulation since finish a round
		if (he_itr == og_he ) break;
	}

	//return (product1 - product2);  
	return (product2 - product1);
}

double ABFpp::CalculateAlphaPara(int t, int k)
{
	double p_alpha = 0;
	double alpha_t_k = 0;
	double beta_t_k = 0;

	alpha_t_k = alpha_[3 * t + k];
	beta_t_k = beta_triangles_[t].angles_[k];

	p_alpha += 2 * (alpha_t_k - beta_t_k) / (beta_t_k * beta_t_k);

	p_alpha += lambda_tri_[t];

	if (!CheckBoundary(t, k))
	{
		p_alpha += lambda_plan_[beta_triangles_[t].index_[k]];
	}

	// which matters is the adjacent angle of alpha(t,k) is boundary
	p_alpha += CalculateAlphaLenPara(t, k);

	return p_alpha;
}

double ABFpp::CalculateTriPara(int f_idx)
{
	return (alpha_[3 * f_idx]+
			alpha_[3 * f_idx + 1]+
			alpha_[3 * f_idx + 2]-
			3.1415926);
}

double ABFpp::ObjectFuncValue()
{
	double result = 0;

	for (int t = 0;t < beta_triangles_.size();++t)
	{
		for (int k = 0;k < 3;++k)
		{
			result += (beta_triangles_[t].angles_[k] - alpha_[3 * t + k]) *
				(beta_triangles_[t].angles_[k] - alpha_[3 * t + k]) /
				(beta_triangles_[t].angles_[k] * beta_triangles_[t].angles_[k]);
		}
	}

	for (int t = 0;t < beta_triangles_.size();++t)
	{
		result += lambda_tri_[t] * CalculateTriPara(t);
	}

	for (int i = 0;i < mesh_->getPointsNumber();++i)
	{
		if (!mesh_->is_boundary(mesh_->vertex_handle(i)))
		{
			result += lambda_plan_[i] * CalculateLenPara(i);
		}
	}

	for (int i = 0;i < mesh_->getPointsNumber();++i)
	{
		if (!mesh_->is_boundary(mesh_->vertex_handle(i)))
		{
			result += lambda_len_[i] * CalculateLenPara(i);
		}
	}

	return result;
}

}
}

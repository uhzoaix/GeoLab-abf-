#include "pluginabfppparametrization.h"

#include <Core/gclnamespace.h>
#include <Core/Utilities/ProgressTool.h>
#include <Core/DataStructures/OMesh.h>

namespace GCL { namespace Plugins{

	typedef OpenMesh::PolyConnectivity::ConstFaceIter ConstFaceIter;

ABFppParametrizationPlugin::ABFppParametrizationPlugin()
{
	this->setObjectName("ABFppParametrization");
 	initParameters();
	valid_mesh_flag_ = false;
}

int ABFppParametrizationPlugin::getFilterType() const
{
	return MeshFilterInterface::FILTER_PARAMETRIZATION;
}

void ABFppParametrizationPlugin::initParameters()
{
	RichParameters& paras = this->Parameters();

	paras.addParameter("Error",(double)0.01,tr("Iteratives Error"),tr("The Error of grad F to 0 "));
	paras.addParameter("Boundary",(int)300000,tr("Boundary Face Number"),tr("The maximum number of using Direct ABF++"));

	paras.setName("ABF++ Parametrization");
	paras.setLabel(tr("ABF++ Parametrization"));
	paras.setIntroduction(tr("Angle Based Flattening Parametrization improved ver."
							 "Contributed by Uhzoaix(Xiangyu Zhu)"));
}

bool ABFppParametrizationPlugin::apply()
{
	if (!this->getMesh()) return false;
	OMesh& mesh = *this->getMesh();

	mesh_ = this->getMesh();
	Init(this->getMesh());										// find triangle and set vector initial value

	// get parameters from GUI
	RichParameters& paras = this->Parameters();

	double epsilon = paras.getValueDouble("Error");
	if (epsilon < 0)
	{
		epsilon = 0.01;
	}

	int f_num = paras.getValueInt("Boundary");
	if (f_num < 0)
	{
		f_num = 30000;
	}
	// apply algorithm to mesh

	if (mesh.getFacesNumber() < f_num)
	{
		// Direct ABF++
		if (valid_mesh_flag_)
		{
			DirectABFpp(mesh, epsilon);
			std::cout << "Invalid Mesh!!" << std::endl;
		}
	}
	else 
	{
		// TODO: Hierarchy ABF++ , but we need simplification algorithm
		;
	}

	// update mesh using the result saved in local vector
	UpdateToParametrization(mesh);
	
	mesh.update_normals();
	updateMesh(Totally_Dirty);

	return true;
}


ABFppParametrizationPlugin::~ABFppParametrizationPlugin()
{
	;
}

// Initialization 

void ABFppParametrizationPlugin::Init(OMesh* mesh)
{
	valid_mesh_flag_ = true;
	
	if (!mesh->is_triangle_mesh())
	{
		//TODO: Warning to Users in UI....
		valid_mesh_flag_ = false;
	}

	if (!has_boundary())
	{
		valid_mesh_flag_ = false;
	}

	FindTriangle(mesh);

	// init the parametrization coordinate vector
	for (int i = 0;i < mesh_->getPointsNumber();++i)
	{
		para_coors_.push_back(Utilities::ParaCoordinate (i,0,0));
	}
}

// find triangle in mesh and calculate its angles
void ABFppParametrizationPlugin::FindTriangle(OMesh* mesh)
{
	for (int i = 0; i < mesh->getFacesNumber(); ++i)
	{
		Vec3i vert_indices = mesh->getFaceTriIndices(i);
		int vi1 = vert_indices[0];
		int vi2 = vert_indices[1];
		int vi3 = vert_indices[2];

		// calculate each triangle's angle and save in mesh_triangles_ 
		MeshTriangle temp_triangle(vi1, vi2, vi3);				// save each vertex's index
		Vec3 v1, v2, v3;
		v1 = mesh->getVertexPosition(vi1);
		v2 = mesh->getVertexPosition(vi2);
		v3 = mesh->getVertexPosition(vi3);
		temp_triangle.InputVertex(v1[0],v1[1],v1[2],
								  v2[0],v2[1],v2[2],
								  v3[0],v3[1],v3[2]);			// calculate angles by coordinate
		mesh_triangles_.push_back(temp_triangle);
	}

	// optimalize interior angles, refer to paper ABF(2001)
	// zhen ta ma keng die
	const double pi = 3.1415926;
	double angle_sum = 0;
	double temp_angle = 0;
	for (int i = 0;i < mesh->getPointsNumber();++i)
	{
		if (!mesh->is_boundary(mesh->vertex_handle(i)))
		{
			// calc angles sum around the v(i)
			OMesh::VertexFaceIter vf0 = mesh_->vf_iter_(mesh_->vertex_handle_(i));
			angle_sum = 0;

			for (; vf0; ++vf0)
			{
				angle_sum += GetAngle((*vf0).idx(), i);
			}
			// adjust each angle around v(i)
			for (vf0 = mesh_->vf_iter_(mesh_->vertex_handle_(i));vf0;++vf0)
			{
				temp_angle = GetAngle((*vf0).idx(), i);
				SetAngle((*vf0).idx(), i, temp_angle * (2 * pi / angle_sum));
			}
		}
	}
}

void ABFppParametrizationPlugin::DirectABFpp(OMesh& mesh,double epsilon)
{
	GCL::Utilities::ABFppEigenSolver solver;				// to compute matrices and solve
	
	Eigen::VectorXd delta(2 * InPointNum(),1);				// delta is iterative change of angles,in paper (6)

	Eigen::VectorXd RHS_mat(2 * InPointNum(),1);
	Utilities::ABFppEigenSolver::MatrixType LHS_mat(2 * InPointNum() , 2 * InPointNum());
	
	delta.setZero();
	RHS_mat.setZero();
	LHS_mat.setZero();

	solver.InitProblem(&mesh,mesh_triangles_,InPointNum());
	
	while (solver.NormGradF() > epsilon)
	{
		std::cout << solver.ObjectFuncValue() << std::endl;
		solver.ComputeMatrix(RHS_mat,LHS_mat);								// update matrix b and J according to alpha and lambda
		solver.Solve(RHS_mat,LHS_mat,delta);
		solver.UpdateAlphaAndLambda(delta);
	}

	// update angles in triangles according to solver' result
	for (int t = 0;t < mesh_triangles_.size();++t)
	{
		for (int k = 0;k < 3;++k)
		{
			mesh_triangles_[t].angles_[k] = solver.GetAngleAlpha(3 * t + k);
		}
	}

	Reconstruct();											// save the Parametrization Coordinates in a local vector
	
}

void ABFppParametrizationPlugin::Reconstruct()
{
	std::vector <OMesh::EdgeHandle> edge_stack;
	
	OMesh::EdgeHandle first_edge = mesh_->edge_handle(0);
	OMesh::HalfedgeHandle first_he = mesh_->halfedge_handle(first_edge,0);
	
	// project first edge
	int temp_v1_idx = mesh_->from_vertex_handle(first_he).idx();
	int temp_v2_idx = mesh_->to_vertex_handle(first_he).idx();
	double length = mesh_->calc_edge_length(first_edge);

	para_coors_[temp_v1_idx].para_coor_x = 0;
	para_coors_[temp_v1_idx].para_coor_y = 0;
	para_coors_[temp_v2_idx].para_coor_x = length;
	para_coors_[temp_v2_idx].para_coor_y = 0;

	// That's to make all nodes projected
	OMesh::HalfedgeHandle temp_he;
	OMesh::HalfedgeHandle temp_he2;

	int temp_f_idx = 0;
	edge_stack.push_back(first_edge);
	
	while (!edge_stack.empty())
	{
		// TODO: to optimize the algorithm
		temp_he = mesh_->halfedge_handle(edge_stack.back(),0);		// Should it be 0? or 1? to  visit the first HE
		temp_he2 = mesh_->halfedge_handle(edge_stack.back(), 1);
		edge_stack.pop_back();

		temp_f_idx = mesh_->face_handle(temp_he).idx();				// what will happen for boundary halfedge??
		if (!mesh_triangles_[temp_f_idx].reconstruct_flag_)
		{
			ProjectNode(temp_he);
			//push other two edges in triangle to stack
			temp_he = mesh_->next_halfedge_handle(temp_he);
			edge_stack.push_back(mesh_->edge_handle(temp_he));
			
			temp_he = mesh_->next_halfedge_handle(temp_he);
			edge_stack.push_back(mesh_->edge_handle(temp_he));

			mesh_triangles_[temp_f_idx].reconstruct_flag_ = true;
		}

		// for another half edge
		temp_f_idx = mesh_->face_handle(temp_he2).idx();	
		if (temp_f_idx >= 0)										// in case of boundary halfhandle
		{
			if (!mesh_triangles_[temp_f_idx].reconstruct_flag_)
			{
				ProjectNode(temp_he2);
				//push other two edges in stack
				temp_he2 = mesh_->next_halfedge_handle(temp_he2);
				edge_stack.push_back(mesh_->edge_handle(temp_he2));
				
				temp_he2 = mesh_->next_halfedge_handle(temp_he2);
				edge_stack.push_back(mesh_->edge_handle(temp_he2));

				mesh_triangles_[temp_f_idx].reconstruct_flag_ = true;
			}
		}
	}
}

// Assisting Functions 

bool ABFppParametrizationPlugin::has_boundary()
{
	OMesh* mesh = this->getMesh();
	for (int i = 0;i < mesh->getPointsNumber();++i)
	{
		if (mesh->is_boundary(mesh->vertex_handle(i)))
		{
			return true;
		}
	}

	return false;
}

unsigned int ABFppParametrizationPlugin::AngleNum()
{
	return (3 * mesh_->getFacesNumber());
}

unsigned int ABFppParametrizationPlugin::ParameterNum()
{
	unsigned int nf = mesh_->getFacesNumber();
	// calculate the number of interior vertex
	int count = 0;											// boundary number
	for (int i = 0; i < mesh_->getPointsNumber() ; ++i)
	{
		if (mesh_-> is_boundary_(mesh_->vertex_handle_(i)))
		{
			count++;
		}
	}
	int ni = mesh_->getPointsNumber() - count;

	return (nf + 2*ni);
}

void ABFppParametrizationPlugin::ProjectNode(OMesh::HalfedgeHandle heh)
{
	// we set v1 as origin ,v1-v2 as x axis
	OMesh::HalfedgeHandle next_he = mesh_->next_halfedge_handle(heh);

	int temp_f_idx = mesh_->face_handle(heh).idx();

	int v1_idx = mesh_->from_vertex_handle(heh).idx();
	int v2_idx = mesh_->to_vertex_handle(heh).idx();
	int v3_idx = mesh_->to_vertex_handle(next_he).idx();

	// bug here!: GetAngle is get the optimal angle
	double alpha1 = GetAngle(temp_f_idx,v1_idx);
	double alpha2 = GetAngle(temp_f_idx,v2_idx);

	double x1 = para_coors_[v1_idx].para_coor_x;
	double y1 = para_coors_[v1_idx].para_coor_y;

	double x2 = para_coors_[v2_idx].para_coor_x;
	double y2 = para_coors_[v2_idx].para_coor_y;

	double length3 = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));

	double length2 = length3 * sin(alpha2) / (sin(alpha1) * cos(alpha2) + sin(alpha2) * cos (alpha1) );
	
	double _x3 = length2 * cos(alpha1);
	double _y3 = length2 * sin(alpha1);

	// coor after displacement and rotation
	double x3 = (_x3 * (x2 - x1) / length3) - (_y3 * (y2 - y1) / length3) + x1;
	double y3 = (_x3 * (y2 - y1) / length3) + (_y3 * (x2 - x1) / length3) + y1;

	para_coors_[v3_idx].para_coor_x = x3;
	para_coors_[v3_idx].para_coor_y = y3;
}

double ABFppParametrizationPlugin::GetAngle(int f_idx, int v_idx)
{
	
	for (int i = 0; i < 3; ++i)
	{
		if (mesh_triangles_[f_idx].index_[i] == v_idx)
		{
			return mesh_triangles_[f_idx].angles_[i];
		}
	}

	return -1;
}

unsigned int ABFppParametrizationPlugin::InPointNum()
{
	// interior points number
	int count = 0;											// boundary number
	for (int i = 0; i < mesh_->getPointsNumber(); ++i)
	{
		if (!mesh_->is_boundary_(mesh_->vertex_handle_(i)))
		{
			count++;
		}
	}

	return (count);
}

void ABFppParametrizationPlugin::UpdateToParametrization(OMesh& mesh)
{
	GCL::Vec3 temp_vec3;

	for (int i = 0;i < mesh.getPointsNumber();++i)
	{
		temp_vec3 = GCL::Vec3(para_coors_[i].para_coor_x, para_coors_[i].para_coor_y, 0);
		mesh.setVertexPosition(i,temp_vec3);
	}
}

void ABFppParametrizationPlugin::SetAngle(int f_idx,int v_idx,double angle)
{
	for (int k = 0;k < 3;++k)
	{
		if (mesh_triangles_[f_idx].index_[k] == v_idx)
		{
			mesh_triangles_[f_idx].angles_[k] = angle;
		}
	}
}

} }
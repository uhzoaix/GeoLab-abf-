#include "FiberMeshFairing.h"
#include <Core/DataStructures/OMesh.h>
#include <Utilities/OpenMeshAlgorithms/LeastSquareLaplacian/LS_LaplacianEigenSolver.h>
namespace GCL { namespace Utilities {

FiberMeshFairing::FiberMeshFairing() :
    lambda_(1),
    iter_Num_(30),
    tolerance_(0.1),
    ls_laplacian_solver_(new  LS_LaplacianEigenSolver<Eigen::SimplicialLDLT<Eigen::SparseMatrix<double> > >())
{
}

bool FiberMeshFairing::fairing(OMesh *mesh)
{
    mesh_ = mesh;
    if(!mesh_) return false;
    for(int iter=0; iter < iter_Num_; iter++)
    {
        if(!iter)
        {
            initLaplacianProblem();
        }

        computeMeanCurvaturePerVertex();
        if(!solveLaplacianCurvatureEnerge())
        {
            DebugLog<<"Error !solveLaplacianCurvatureEnerge()"<<std::endl;
            return false;
        }
        computeLaplacianCoordinatesPerVertex();
        if(!solveLaplacianPositionEnerge())
        {
            DebugLog<<"Error !solveLaplacianPositionEnerge()"<<std::endl;
            return false;
        }
    }
    return true;
}

bool FiberMeshFairing::initLaplacianProblem()
{
    return ls_laplacian_solver_->compute(*mesh_,lambda_,false,false);
}

void FiberMeshFairing::computeMeanCurvaturePerVertex()
{
    c_.clear();
    for(size_t i=0; i < mesh_->n_vertices_(); i++)
    {

        OMesh::VertexHandle vh = mesh_->vertex_handle_(i);
        Vec3 center;
        int count  = 0;
        for(OMesh::ConstVertexVertexIter cvvitr = mesh_->cvv_iter_(vh); cvvitr; cvvitr++)
        {
            if(mesh_->isVertexSelected(i))
            {
                if(mesh_->isVertexSelected(cvvitr->idx()))
                {
                    center = center + mesh_->point_(*cvvitr);
                    count++;
                }
            }
            else
            {
                if(!onlyOnSelectedVertices_)
                {
                    center = center + mesh_->point_(*cvvitr);
                    count++;
                }
            }
        }
        if(count > 0)
        {
            center = center * (1 / (Scalar)count);
            c_.push_back((mesh_->point_(vh) - center).length());
            //            DebugLog<<c_.back()<<std::endl;
        }
        else
        {
            c_.push_back(0);
        }
    }
}

int FiberMeshFairing::solveLaplacianCurvatureEnerge()
{
    std::vector< Scalar > lb(mesh_->n_vertices_());
    std::vector< Scalar > x(mesh_->n_vertices_());
    if(!ls_laplacian_solver_->solve(lb,c_,x))
    {
        return 0;
    }
    Scalar re = 0;
    Scalar total = 0;
    for(size_t i=0; i < c_.size(); i++)
    {
        re += sqrt( (c_[i] - x[i]) * (c_[i] - x[i]));
        total += sqrt(c_[i] * c_[i]);
    }

    DebugLog<<re / total <<std::endl;
    if(re < total * tolerance_)
    {
        return -1;
    }
    c_.clear();

    for(size_t i=0; i < mesh_->n_vertices_(); i++)
    {
        c_.push_back(x[i]);
    }
    return 1;
}

void FiberMeshFairing::computeLaplacianCoordinatesPerVertex()
{
    Lv_.clear();
    for(size_t i=0; i < mesh_->n_vertices_(); i++)
    {
        OMesh::VertexHandle vh = mesh_->vertex_handle_(i);
        Vec3 lv = mesh_->normal_(vh) * c_[i];
        Lv_.push_back(lv);
    }
}

bool FiberMeshFairing::solveLaplacianPositionEnerge()
{
    std::vector< Vec3 > x(mesh_->n_vertices_());
    std::vector< Vec3 > b;
    for(size_t i=0; i < mesh_->n_vertices_(); i++)
    {
        OMesh::VertexHandle vh = mesh_->vertex_handle_(i);

        b.push_back(mesh_->point_(vh));

    }
    if(!ls_laplacian_solver_->solve(Lv_,b,x)) return false;
    for(size_t i=0; i < mesh_->n_vertices_(); i++)
    {
        mesh_->point_(mesh_->vertex_handle_(i)) = x[i];
    }
    mesh_->update_normals();
    return true;
}


}}

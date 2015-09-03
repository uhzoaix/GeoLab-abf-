#include "LeastSquareLaplacian.h"
#include <Core/DataStructures/OMesh.h>
namespace GCL { namespace Utilities {
LeastSquareLaplacian::LeastSquareLaplacian():mesh_(NULL)
{
}

bool LeastSquareLaplacian::compute(const OMesh &mesh, Scalar _lambda, bool constraints_on_selected_point_only, bool is_hard_constraints)
{
    mesh_ = &mesh;
    lambda_ = _lambda;
    is_hard_constraints_ = is_hard_constraints;
    constraints_on_selected_point_only_ = constraints_on_selected_point_only;
    initProblemSize(mesh);
    return compute_matrices(mesh);
}

void LeastSquareLaplacian::initProblemSize(const OMesh &mesh)
{
    if(is_hard_constraints_)
    {
        v_index_.clear();
        n_cols_ = 0;
        for(size_t i=0; i < mesh.n_vertices_(); i++)
        {
            if(mesh.isVertexSelected(i))
            {
                v_index_.push_back(n_cols_ - mesh.n_vertices_());
            }
            else
            {
                v_index_.push_back(n_cols_++);
            }
        }
        n_L_rows_ = mesh.n_vertices_();
        if(constraints_on_selected_point_only_)
        {
            n_I_rows_ = 0;
        }
        else
        {
            n_I_rows_ = n_cols_;
        }

    }
    else
    {
        n_cols_ = mesh.n_vertices_();
        n_L_rows_ = n_cols_;
        n_I_rows_ = 0;
        if(constraints_on_selected_point_only_)
        {
            for(size_t i=0; i < mesh.n_vertices_(); i++)
            {
                if(mesh.isVertexSelected(i))
                {
                    n_I_rows_++;
                }
            }
        }
        else
        {
            n_I_rows_ = n_cols_;
        }
    }
}



void LeastSquareLaplacian::initLaplaceMatrix(const OMesh &mesh, RowMaps &Lmap)
{
    if(is_hard_constraints_)
    {
        Lmap.clear();
        for(size_t i=0; i < mesh.n_vertices_(); i++)
        {
            std::map<int, Scalar> rmap;
            OMesh::VertexHandle vh = mesh.vertex_handle_(i);
            if(v_index_[i] >= 0)
            {
                rmap[ v_index_[i] ] = 1;
            }
            int d = mesh.valence_(vh);
            if(d > 0)
            {
                for(OMesh::ConstVertexVertexIter cvvitr=mesh.cvv_iter_(vh);cvvitr; cvvitr++)
                {
                    if(v_index_[cvvitr->idx()] >= 0)
                    {
                        rmap[v_index_[cvvitr->idx()]] = - 1.0 / (Scalar) d;
                    }
                }
            }
            Lmap.push_back(rmap);
        }
    }
    else
    {
        Lmap.clear();
        for(size_t i=0; i < mesh.n_vertices_(); i++)
        {
            std::map<int, Scalar> rmap;
            OMesh::VertexHandle vh = mesh.vertex_handle_(i);
            rmap[i] = 1;
            int d = mesh.valence_(vh);
            //        DebugLog<<i<<" ("<<d<<"): ";
            if(d > 0)
            {
                for(OMesh::ConstVertexVertexIter cvvitr=mesh.cvv_iter_(vh);cvvitr; cvvitr++)
                {
                    //                DebugLog<<" "<<cvvitr->idx();
                    rmap[cvvitr->idx()] = - 1.0 / (Scalar) d;
                }
            }
            Lmap.push_back(rmap);
            //        DebugLog<<std::endl;
        }
    }
}

void LeastSquareLaplacian::initContraintsMatrix(const OMesh &mesh, RowMaps &Imap)
{
    if(is_hard_constraints_)
    {
        Imap.clear();
        if(constraints_on_selected_point_only_) return;
        for(int i=0; i < n_cols_; i++)
        {
            std::map< int, Scalar> rmap;
            rmap[i] = 1;
            Imap.push_back(rmap);
        }
    }
    else
    {
        Imap.clear();

        for(size_t i=0; i < mesh.n_vertices_(); i++)
        {
            if(mesh.isVertexSelected(i))
            {
                std::map< int, Scalar> rmap;
                rmap[i] = 2;
                Imap.push_back(rmap);
            }
            else
            {
                if(!constraints_on_selected_point_only_)
                {
                    std::map< int, Scalar> rmap;
                    rmap[i] = 1;
                    Imap.push_back(rmap);
                }
            }
        }
    }
}

bool LeastSquareLaplacian::preprocess_LaplacianVector(std::vector<Scalar> &lb, const std::vector<Scalar> &b)
{
    if(is_hard_constraints_)
    {
        if(lb.size() != mesh_->n_vertices_()) return false;
        if(b.size() != mesh_->n_vertices_()) return false;
        for(size_t i=0; i < mesh_->n_vertices_(); i++)
        {
            OMesh::VertexHandle vh = mesh_->vertex_handle_(i);
            if(mesh_->isVertexSelected(i))
            {
                lb[i] -= b[i];
            }
            int d = mesh_->valence_(vh);
            if(d > 0)
            {
                for(OMesh::ConstVertexVertexIter cvvitr=mesh_->cvv_iter_(vh);cvvitr; cvvitr++)
                {
                    if(mesh_->isVertexSelected(cvvitr->idx()))
                    {
                        lb[cvvitr->idx()] -= (- 1.0 / (Scalar) d) * b[cvvitr->idx()];
                    }
                }
            }
        }


    }
    return true;
}

bool LeastSquareLaplacian::preprocess_ConstraintsVector(std::vector<Scalar> &b)
{
    if(is_hard_constraints_)
    {
        if(b.size() != mesh_->n_vertices_()) return false;
        b_constraints_ = b;
        if(constraints_on_selected_point_only_)
        {
            b.clear();
            return true;
        }
        std::vector<Scalar> tmp;
        for(size_t i=0; i < v_index_.size(); i++)
        {
            if(v_index_[i] >= 0)
            {
                tmp.push_back(b[i]);
            }
        }
        b = tmp;
    }
    else
    {
        for(size_t i=0; i < b.size(); i++)
        {
            if(constraints_on_selected_point_only_
                    || mesh_->isVertexSelected(i))
            {
                b[i] *= 2;
            }
        }
    }
    return true;
}

bool LeastSquareLaplacian::postprocess_ResultVector(std::vector<Scalar>& x)
{
    if(is_hard_constraints_)
    {
        std::vector<Scalar> tmp;
        for(size_t i=0; i < v_index_.size(); i++)
        {
            if(v_index_[i] >= 0)
            {
                tmp.push_back(x[v_index_[i]]);
            }
            else
            {
                tmp.push_back(b_constraints_[i]);
            }
        }
        x = tmp;
    }
    return true;
}


}}

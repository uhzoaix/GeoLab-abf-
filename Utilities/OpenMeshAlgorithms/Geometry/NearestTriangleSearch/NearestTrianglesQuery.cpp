#include "NearestTrianglesQuery.h"
#include <Core/DataStructures/OMesh.h>
#include <set>
namespace GCL { namespace Utilities {


NearestTrianglesFinder::NearestTrianglesFinder():root_(NULL),mesh_(NULL),max_depth_(10000)
{

}

NearestTrianglesFinder::~NearestTrianglesFinder()
{
    if(root_)
    {
        destroyNode(root_);
        root_ = NULL;
    }
}

void NearestTrianglesFinder::build_tree(OMesh *mesh)
{
    if(!mesh) return;
    if(root_)
    {
        destroyNode(root_);
        root_ = NULL;
    }
    mesh_ = mesh;
    face_handles_.clear();
    tmp_face_handles_.clear();
    for(size_t i=0; i < mesh_->getFacesNumber(); i++)
    {
        face_handles_.push_back(mesh_->face_handle_(i));
        tmp_face_handles_.push_back(mesh_->face_handle_(i));
    }
    root_ = build_tree_node(0,mesh_->getFacesNumber() - 1, 0);

}

void NearestTrianglesFinder::query_on_tree(NeareastTrianglesQuery *query)
{
    if(root_) query_on_tree_node(root_,query);
}

void NearestTrianglesFinder::query_on_tree_node(KDTreeNode *node, NeareastTrianglesQuery *query)
{
    if(node->isLeaf)
    {
        if(node->faceHandles)
        {
            for(size_t i=0; i < node->faceHandles->size(); i++)
            {
                query->query_on_face(mesh_,node->faceHandles->at(i));
            }
        }
        return;
    }

    if(!query->check_whether_to_search_kdtree_node(node)) return;

    if(node->leftChild) this->query_on_tree_node(node->leftChild,query);
    if(node->rightChild) this->query_on_tree_node(node->rightChild,query);

}

void NearestTrianglesFinder::destroyNode(KDTreeNode *node)
{
    if(node->isLeaf)
    {
        if (node->faceHandles!=NULL)
        {
            node->faceHandles->clear();
            delete node->faceHandles;
            node->faceHandles=NULL;
        }
    }
    else
    {
        if(node->leftChild) destroyNode(node->leftChild);
        if(node->rightChild) destroyNode(node->rightChild);
        node->isLeaf = true;
    }
    delete node;
}

KDTreeNode *NearestTrianglesFinder::build_tree_node(int _l, int _r, int depth)
{
    if(_l > _r) return NULL;

    //*0:  new KDTreeNode
    KDTreeNode* node = new KDTreeNode();



    //*1:  compute bounding box
    Vec3 vmin,vmax;
    for(int i = _l; i <= _r; i++)
    {
        OMesh::FaceHandle fh =  face_handles_[i];
        Vec3 v0,v1,v2;
        this->get_positions_on_face(fh,v0,v1,v2);
        Vec3 face_center = (v0+v1+v2) * (1.0 / 3.0);
        if( i == _l)
        {
            vmin = face_center;
            vmax = face_center;
        }
        else
        {
            vmin.min(face_center);
            vmax.max(face_center);
        }
    }
    node->boundingBox.maxCorner = vmax;
    node->boundingBox.minCorner = vmin;

    //*2: check if it is a leaf
    if(_r == _l || depth >= max_depth_)
    {
        node->isLeaf = true;
        node->faceHandles = new std::vector<OMesh::FaceHandle>();
        for(int i=_l; i <= _r; i++)
        {
            node->faceHandles->push_back(face_handles_[i]);
        }
        node->leftChild = NULL;
        node->rightChild = NULL;
        node->splittingPlaneCoordinate = vmin[X_AXIS];
        node->coordinateAxisOfSplitting = X_AXIS;
        return node;
    }

    node->isLeaf = false;

    //*3: set splitting plane and axis
    Vec3 vtmp = vmax - vmin;
    Vec3 vcenter = (vmax + vmin) * 0.5;
    node->coordinateAxisOfSplitting = vtmp.maxabsaxis();

    node->splittingPlaneCoordinate = vcenter[node->coordinateAxisOfSplitting];


    //*4: compute radius
    node->radius = 0.0;
    node->center = vcenter;
    for(int i=_l; i <= _r; i++)
    {
        OMesh::FaceHandle fh =  face_handles_[i];
        Vec3 v0,v1,v2;
        this->get_positions_on_face(fh,v0,v1,v2);
        node->radius = ((v0 - vcenter).length() > node->radius)?(v0 - vcenter).length():node->radius;
        node->radius = ((v1 - vcenter).length() > node->radius)?(v1 - vcenter).length():node->radius;
        node->radius = ((v2 - vcenter).length() > node->radius)?(v2 - vcenter).length():node->radius;
    }



    //4: resort the vector and build children
    int l_count = 0;
    int r_count = 0;
    for(int i = _l; i <= _r; i++)
    {
        OMesh::FaceHandle fh =  face_handles_[i];
        Vec3 v0,v1,v2;
        this->get_positions_on_face(fh,v0,v1,v2);
        Vec3 face_center = (v0+v1+v2) * (1.0 / 3.0);
        if(face_center[node->coordinateAxisOfSplitting] <= node->splittingPlaneCoordinate)
        {
            tmp_face_handles_[_l + l_count] = fh;
            l_count++;
        }
        else
        {
            tmp_face_handles_[_r - r_count] = fh;
            r_count++;
        }
    }
    for(int i=_l; i <= _r; i++)
    {
        face_handles_[i] = tmp_face_handles_[i];
    }

    //5: get children
    node->leftChild = build_tree_node(_l,_l + l_count-1,depth + 1);
    node->rightChild = build_tree_node(_l + l_count, _r,depth+1);

    return node;

}

void NearestTrianglesFinder::get_positions_on_face(OMesh::FaceHandle _fh, Vec3 &v0, Vec3 &v1, Vec3 &v2)
{
    if(!mesh_) return;
    int count = 0;
    for(OMesh::FaceVertexIter itr = mesh_->fv_iter_(_fh); itr.is_valid(); itr++)
    {
        if(count == 0) v0 = mesh_->point_(itr.handle());
        if(count == 1) v1 = mesh_->point_(itr.handle());
        if(count == 2) v2 = mesh_->point_(itr.handle());
        count++;
    }
}

ClosestPointQuery::ClosestPointQuery(const Vec3 &_point):point_(_point),dist_(-1),count_(0)
{

}

void ClosestPointQuery::init(const Vec3 &_point)
{
    point_ = _point;
    dist_ = -1;
    count_ = 0;
}

void ClosestPointQuery::query_on_face(OMesh *mesh, OMesh::FaceHandle fh)
{
    if(!mesh) return;
    count_++;
    for(OMesh::FaceVertexIter itr = mesh->fv_iter_(fh); itr.is_valid(); itr++)
    {
        Vec3 v = mesh->point_(itr.handle());
        if(dist_ < 0 || (v - point_).length() < dist_)
        {
            dist_ = (v-point_).length();
            ans_ = v;
        }

    }
}



bool ClosestPointQuery::check_whether_to_search_kdtree_node(KDTreeNode *node)
{
    if(!node) return false;
    if(dist_ < 0) return true;
    if( (node->center - point_).length() >= dist_ + node->radius + TOLERANCE)
    {
        return false;
    }
    return true;
}

TriangleIntersectionQuery::TriangleIntersectionQuery(const Vec3 &v0, const Vec3 &v1, const Vec3 &v2)
{
    init(v0,v1,v2);
}

TriangleIntersectionQuery::~TriangleIntersectionQuery()
{

}

void TriangleIntersectionQuery::init(const Vec3 &v0, const Vec3 &v1, const Vec3 &v2)
{
    facehandles_.clear();
    v0_ = v0; v1_ = v1; v2_ = v2; count_ = 0;
}

void TriangleIntersectionQuery::query_on_face(OMesh *mesh, OMesh::FaceHandle fh)
{

    if(!mesh) return;
    count_++;
    Vec3 v0[3];
    int vcount = 0;
    for(OMesh::FaceVertexIter itr = mesh->fv_iter_(fh); itr.is_valid(); itr++)
    {
        v0[vcount++] = mesh->point_(*itr);
    }
    Vec3 v1[3];
    v1[0] = v0_; v1[1] = v1_; v1[2] = v2_;

    for(int i=0; i < 3; i++)
    {
        Vec3 vdir = (v0[(i+1) % 3] - v0[i]).normalize();
        Vec3 intersect;
        if(Vec3::getIntersectionRayToTriangle(v0[i],vdir,v1[0],v1[1],v1[2],intersect))
        {
            Scalar dist0 = (v0[(i+1) % 3] - v0[i]) * vdir;
            Scalar dist1 = (intersect - v0[i]) * vdir;
            if(dist1 >= -TOLERANCE && dist1 < dist0 + TOLERANCE)
            {
                facehandles_.push_back(fh);
                return;
            }
        }


    }

    for(int i=0; i < 3; i++)
    {
        Vec3 vdir = (v1[(i+1) % 3] - v1[i]).normalize();
        Vec3 intersect;
        if(Vec3::getIntersectionRayToTriangle(v1[i],vdir,v0[0],v0[1],v0[2],intersect))
        {
            Scalar dist0 = (v1[(i+1) % 3] - v1[i]) * vdir;
            Scalar dist1 = (intersect - v1[i]) * vdir;
            if(dist1 >= -TOLERANCE && dist1 < dist0 + TOLERANCE)
            {
                facehandles_.push_back(fh);
                return;
            }
        }

    }

}

bool TriangleIntersectionQuery::check_whether_to_search_kdtree_node(KDTreeNode *node)
{
    Vec3 normal = (v1_ - v0_) ^ (v2_ - v0_);
    normal.Normalize();
    return (fabs((node->center - v0_) * normal) <= node->radius + TOLERANCE);

}

void TriangleIntersectionQuery::print()
{

    std::set<int> t_set;
    for(size_t i=0; i < facehandles_.size(); i++)
    {
        t_set.insert(facehandles_[i].idx());
    }
    for(std::set<int>::iterator itr= t_set.begin(); itr != t_set.end(); itr++)
    {
        DebugLog<<*itr<<" ";
    }
    DebugLog<<std::endl;
}

}}

#include "OMeshCtrlPoints.h"

namespace GCL {

OMeshCtrlPoints::OMeshCtrlPoints(const std::shared_ptr<OMesh> &_mesh)
    :mesh_(_mesh)
{
}

OMeshCtrlPoints::OMeshCtrlPoints( OMeshCtrlPoints &other)
{
    mesh_ = other.getMesh();
    point_handlers_.clear();
    for(int i=0; i < other.getPointHandlersNumber(); i++)
    {
        point_handlers_.push_back(other.getPointHandle(i));
    }
}

void OMeshCtrlPoints::addPointHandle(const OMeshCtrlPoints::PointHandle &pointHandle)
{
    point_handlers_.push_back(pointHandle);
}

void OMeshCtrlPoints::addPointHandle(const OMesh::VertexHandle &_vh)
{
    if(mesh_.get() == NULL) return;
    point_handlers_.push_back(PointHandle((*mesh_),_vh));
}

void OMeshCtrlPoints::generateFromSelectedPoints()
{
    if(mesh_.get() == NULL) return;
    point_handlers_.clear();
    for(OMesh::VertexIter itr = mesh_->vertices_begin_(); itr != mesh_->vertices_end_(); itr++)
    {
        if(mesh_->status((*itr)).selected())
        {
            addPointHandle((*itr));
        }
    }
}

}

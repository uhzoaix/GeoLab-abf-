#include "MeshFilterInterface.h"
#include <Core/SceneGraph/SceneGraph.h>
#include <Core/SceneGraph/OpenMeshNode.h>
#include <Core/DataStructures/OMesh.h>
namespace GCL { namespace Plugins {

MeshFilterInterface::MeshFilterInterface(QObject *parent)
    :PluginInterface(parent)
{
}

MeshFilterInterface::~MeshFilterInterface()
{

}

QString MeshFilterInterface::getCategory() const
{
    return getFilterCategoryName((size_t)getFilterType());
}

QString MeshFilterInterface::getFilterCategoryName(size_t i)
{
    switch (i) {
    case FILTER_REPAIRING:
        return "Repairing";
        break;
    case FILTER_REMESHING:
        return "Remeshing Simplification and Subdivision";
        break;
    case FILTER_SMOOTHING:
        return "Smoothing Denoising and Fairing";
        break;
    case FILTER_RECONSTRUCTION:
        return "Reconstruction";
        break;
    case FILTER_SAMPLING:
        return "Sampling";
        break;
    case FILTER_PARAMETRIZATION:
        return "Parametrization";
        break;
    default:
        return "Others";
        break;
    }
}

OMesh *MeshFilterInterface::getMesh()
{
    if(!getSceneGraph()) return NULL;
    SceneGraph::OpenMeshNode* omesh_node = getSceneGraph()->getFirstDescendant<SceneGraph::OpenMeshNode* >(true);
    if(!omesh_node)
    {
        omesh_node =  getSceneGraph()->getFirstDescendant<SceneGraph::OpenMeshNode* >(false);
        if(!omesh_node){
            return NULL;
        }
    }
    return omesh_node->getMesh().get();
}

void MeshFilterInterface::updateMesh(int dirtyType)
{
    if(!getSceneGraph()) return;
    SceneGraph::OpenMeshNode* omesh_node = getSceneGraph()->getFirstDescendant<SceneGraph::OpenMeshNode* >(true);
    if(!omesh_node)
    {
        omesh_node = getSceneGraph()->getFirstDescendant<SceneGraph::OpenMeshNode* >(false);
        if(!omesh_node){
            return;
        }
    }
    omesh_node->nodeUpdated(dirtyType);
}








}}

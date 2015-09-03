#include "NodeFactory.h"
#include "GroupNode.h"
#include <OpenMesh/Core/IO/IOInstances.hh>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <QFileInfo>
#include <memory>
#include "OpenMeshNode.h"
#include "../DataStructures/OMesh.h"

#include "OMeshCtrlPointsNode.h"
#include "../DataStructures/OMeshCtrlPoints.h"
namespace GCL { namespace SceneGraph {

GroupNode *NodeFactory::createGroupNode(Node *parent)
{
    /// We don't create Node without SceneGraph root node;
    if(!parent) return NULL;
    return (new GroupNode(parent));
}

OpenMeshNode *NodeFactory::createOpenMeshNode(const QString &filename, Node *parent)
{
    if(!parent) return NULL;
    std::shared_ptr<OMesh> mesh(new OMesh);
    if(OpenMesh::IO::read_mesh((*mesh),filename.toStdString()))
    {
        mesh->update_normals();
        mesh->MeshNormalize();
        OpenMeshNode* pPointer = new OpenMeshNode(mesh,parent);
        return pPointer;
    }
    return NULL;
}

OMeshCtrlPointsNode *NodeFactory::createOMeshCtrlPointsNode(OpenMeshNode *parent)
{
    if(!parent) return NULL;
    return (new OMeshCtrlPointsNode(std::shared_ptr<OMeshCtrlPoints>(new OMeshCtrlPoints(parent->getMesh())),parent));

}

DepthImageNode *NodeFactory::createDepthImageNode(const QString &filename, Node *parent)
{
    return NULL;
}



}}

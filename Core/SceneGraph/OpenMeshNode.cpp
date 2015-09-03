#include "OpenMeshNode.h"
#include "../DataStructures/OMesh.h"
#include "NodeFactory.h"
#include "NodeAction.h"
#include "SceneGraph.h"
#include "OMeshCtrlPointsNode.h"
namespace GCL{ namespace SceneGraph {

OpenMeshNode::OpenMeshNode(const std::shared_ptr<OMesh> &_mesh, Node *parent)
    :mesh_(_mesh),GeometryNode(parent)
{
//    this->setName("OpenMeshNode");
//    std::cout<<"OpenMeshNode verticesNumber: "<<mesh_->getPointsNumber()<<std::endl;
//    std::cout<<"OpenMeshNode faceNumber: "<<mesh_->getFacesNumber()<<std::endl;
    this->addNodeAction(std::shared_ptr<NodeAction>(new NodeAction("CreateCtrlPointsNode",false,tr("Create CtrlPoints Node"),"")));
}

GeometryBase *OpenMeshNode::getGeometry()
{

    return mesh_.get();
}



void OpenMeshNode::slotAction(const QString &action_name)
{
    GeometryNode::slotAction(action_name);

    if(action_name == "CreateCtrlPointsNode")
    {
        if(getScenegraph() != NULL)
        {
            SceneGraphNode* root = static_cast<SceneGraphNode*>(getScenegraph());
            root->getNodeFactory()->createOMeshCtrlPointsNode(this)->setName(tr("Ctrl Points"));
            root->nodeUpdated();
        }
    }

}


}}

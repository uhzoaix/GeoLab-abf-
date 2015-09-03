#include "OMeshCtrlPointsNode.h"
#include "NodeAction.h"
#include "../DataStructures/OMeshCtrlPoints.h"
namespace GCL { namespace SceneGraph {

OMeshCtrlPointsNode::OMeshCtrlPointsNode(const std::shared_ptr<OMeshCtrlPoints> &_ctrlpoints, Node *parent)
    :ctrl_points_(_ctrlpoints),CtrlPointsNode(parent)
{
    this->addNodeAction(std::shared_ptr<NodeAction>(new NodeAction("GenerateCtrlPointsFromSelected",false,tr("Generate CtrlPoints From Selected"),"")));
}

OMeshCtrlPointsNode::~OMeshCtrlPointsNode()
{

}

GeometryBase *OMeshCtrlPointsNode::getGeometry()
{
    return ctrl_points_.get();
}

void OMeshCtrlPointsNode::slotAction(const QString &action_name)
{
    GeometryNode::slotAction(action_name);
    if(action_name == "GenerateCtrlPointsFromSelected")
    {
        ctrl_points_->generateFromSelectedPoints();
        this->nodeUpdated(Totally_Dirty);
    }
}

}}

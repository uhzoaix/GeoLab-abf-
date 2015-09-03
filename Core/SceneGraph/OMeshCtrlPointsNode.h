#ifndef GCL_OMESHCTRLPOINTSNODE_H
#define GCL_OMESHCTRLPOINTSNODE_H
#include "CtrlPointsNode.h"
#include <memory>
namespace GCL {

class OMeshCtrlPoints;
namespace SceneGraph {

class GCL_CORE_SHARED_EXPORT OMeshCtrlPointsNode : public CtrlPointsNode
{
    Q_OBJECT
public:
    OMeshCtrlPointsNode(const std::shared_ptr<OMeshCtrlPoints>& _ctrlpoints, Node* parent);
    virtual ~OMeshCtrlPointsNode();
    virtual QString getNodeTypeName() const {return tr("OMeshCtrlPointsNode");}
    virtual GeometryBase* getGeometry();
public slots:
    virtual void slotAction(const QString &action_name);
private:
    std::shared_ptr<OMeshCtrlPoints> ctrl_points_;
};

}} // namespace
#endif // OMESHCTRLPOINTSNODE_H

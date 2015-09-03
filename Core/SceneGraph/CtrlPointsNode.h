#ifndef GCL_SCENEGRAPH_CTRLPOINTSNODE_H
#define GCL_SCENEGRAPH_CTRLPOINTSNODE_H
#include "gcl_global.h"
#include "GeometryNode.h"
#include "../Math/MathDefines.h"
namespace GCL { namespace SceneGraph {
class GCL_CORE_SHARED_EXPORT CtrlPointsNode : public GeometryNode
{
    Q_OBJECT
public:
    explicit CtrlPointsNode(Node* parent = 0);
    virtual ~CtrlPointsNode();
    virtual QString getNodeTypeName() const {return tr("CtrlPointsNode");}
    virtual GeometryBase* getGeometry() = 0;

    virtual void  render(RenderModes *pRenderModes);
};
}}
#endif // CTRLPOINTSNODE_H

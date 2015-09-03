#ifndef NODEFACTORY_H
#define NODEFACTORY_H
#include "Core/gcl_global.h"
#include <QString>
namespace GCL{ namespace SceneGraph {
class Node;
class GroupNode;
class OpenMeshNode;
class DepthImageNode;
class OMeshCtrlPointsNode;
class GCL_CORE_SHARED_EXPORT NodeFactory
{
public:
    NodeFactory() {}
    virtual ~NodeFactory() {}

    virtual GroupNode* createGroupNode(Node* parent);

    /// Create OpenMesh Node by filename
    virtual OpenMeshNode* createOpenMeshNode(const QString& filename,Node* parent);

    virtual OMeshCtrlPointsNode* createOMeshCtrlPointsNode(OpenMeshNode* parent);

    virtual DepthImageNode* createDepthImageNode(const QString& filename,Node* parent);
};
}}
#endif // NODEFACTORY_H

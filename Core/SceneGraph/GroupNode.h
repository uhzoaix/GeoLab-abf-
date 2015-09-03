#ifndef GCL_GROUPNODE_H
#define GCL_GROUPNODE_H
#include "Node.h"
namespace GCL { namespace SceneGraph {


class GCL_CORE_SHARED_EXPORT GroupNode : public Node
{
public:
    explicit GroupNode(Node *parent = 0);
    virtual ~GroupNode() {}
    //  return the basic nodetype defined in NodeType.h
    int getNodeType() const;
    //  return the node type name : it can be diffrent with nodetype
    virtual QString getNodeTypeName() const;
    ////visitor pattern
    virtual void accept(Visitor & _visitor);

};


}
}


#endif // GROUP_H

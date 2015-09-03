#include "GroupNode.h"
#include "NodeType.h"
#include "Visitor.h"
namespace GCL {namespace SceneGraph {

GroupNode::GroupNode(Node *parent)
    :Node(parent)
{
    this->setName("Group");
}

int GroupNode::getNodeType() const
{
    return GROUP_NODE;
}

QString GroupNode::getNodeTypeName() const
{
    return "Group";
}

void GroupNode::accept(Visitor &_visitor)
{
    if(_visitor.enter(this)) {
      for(int i=0; i<getNumberOfChildren(); ++i) {
        getChild(i)->accept(_visitor);
      }
    }
    _visitor.leave(this);
}


}}

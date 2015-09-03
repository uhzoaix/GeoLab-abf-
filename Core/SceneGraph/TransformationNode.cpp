#include "TransformationNode.h"
#include "NodeType.h"
#include "qgl.h"
#include "Visitor.h"
namespace GCL { namespace SceneGraph {

TransformationNode::TransformationNode(Node *parent)
    :Node(parent),scale_(1),offset_(),rotation_(),render_type_(TR_NONE),current_axis_for_render_(0)
{

}

TransformationNode::~TransformationNode()
{

}

int TransformationNode::getNodeType() const
{
    return TRANSFORMATION_NODE;
}

QString TransformationNode::getNodeTypeName() const
{
    return "TransformationNode";
}

void TransformationNode::accept(Visitor &_visitor)
{
    if(_visitor.enter(this)) {
      for(int i=0; i<getNumberOfChildren(); ++i) {
        getChild(i)->accept(_visitor);
      }
    }
    _visitor.leave(this);
}
Scalar TransformationNode::getScale() const
{
    return scale_;
}

void TransformationNode::setScale(const Scalar &scale)
{
    scale_ = scale;
}
Vec3 TransformationNode::getOffset() const
{
    return offset_;
}

void TransformationNode::setOffset(const Vec3 &offset)
{
    offset_ = offset;
}
Quat TransformationNode::getRotation() const
{
    return rotation_;
}

void TransformationNode::setRotation(const Quat &rotation)
{
    rotation_ = rotation;
}

void TransformationNode::render_enter()
{
    glPushMatrix();
    glScaled(scale_,scale_,scale_);
    glRotated(rotation_.getAngle(),rotation_.getAxis().at(0),rotation_.getAxis().at(1),rotation_.getAxis().at(2));
    glTranslated(offset_[0],offset_[1],offset_[2]);
}


void TransformationNode::render_leave()
{
    glPopMatrix();
}
int TransformationNode::get_render_type() const
{
    return render_type_;
}

void TransformationNode::set_render_type(int render_type)
{
    render_type_ = render_type;
}












}}

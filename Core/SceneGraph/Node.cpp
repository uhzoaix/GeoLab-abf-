#include "Node.h"
#include "RenderModes.h"
#include "NodeAction.h"
#include "Utilities/LogSystem.h"
#include "DataStructures/AABB.h"
namespace GCL { namespace SceneGraph {


Node::Node(QObject *parent)
    :QObject(parent),selected_(false),visible_(true),
      render_modes_(new RenderModes(this)),render_setting_overriding_(false),
      aabb_(new AABB()),aabbIsDirty_(false)
{
    markBoundingVolumesDirty();
    initNodeActions();
}

Node::~Node()
{
}

void Node::setName(const QString &_name)
{
    QString name=_name;
    Node* sg=getScenegraph();
    Node* n = sg->findChild<Node*>(name);
    int c=0;

    //automatically append the smallest integer to the name to make it unique
    while( n ){
        name=_name;
        name.append(QString::number(c));
        c++;
        n = sg->findChild<Node*>(name);
    }

    setObjectName(name);
}



Node *Node::getScenegraph()
{
    if(hasParent())
    {
        return getParent()->getScenegraph();
    }
    else
    {
        return this;
    }

}

int Node::getChildIndex(const Node * const _child) const
{
    int index = -1;
    int i = 0;

    QList<Node*> nodeChildren = findChildren<Node*>();
    for(QList<Node*>::iterator it=nodeChildren.begin(); it!=nodeChildren.end(); ++it)
    {
        if(*it == _child) {
            index = i;
            break;
        }
        ++i;
    }

    return index;
}

void Node::nodeUpdated(int dirtyType)
{
    if(hasParent())
    {
        getParent()->nodeUpdated();
    }
}

void Node::setRenderSettingsOverriding(bool _isOverriding)
{
    if(_isOverriding != render_setting_overriding_)
    {
        render_setting_overriding_ = _isOverriding;
    }
}



////bounding box
void Node::recomputeAABB()
{


    aabb_->setEmpty();

    //collect bounding boxes of all children
    for(int i=0; i<getNumberOfChildren(); ++i)
    {
        aabb_->extend(*getChild(i)->getAABB());
    }

    aabbIsDirty_  = false;
}

bool Node::isDirtyAABB() const
{
    return aabbIsDirty_;
}

const AABB* Node::getAABB()
{
    if (isDirtyAABB())
    {
        recomputeAABB();
    }

    return aabb_.get();
}


////general bounding volume routines
void Node::markBoundingVolumesDirty()
{

    aabbIsDirty_ = true;
    if(hasParent())
    {
        getParent()->markBoundingVolumesDirty();
    }

}

void Node::initNodeActions()
{
    if(getParent() != NULL)
    {
        actions_.push_back(std::shared_ptr<NodeAction>(new NodeAction("RenderSettingOverride",true,tr("Override Setting"))));
    }

    actions_.push_back(std::shared_ptr<NodeAction>(new NodeAction("VertexVisible",true,tr("Visible"),tr("Vertices"))));
    actions_.push_back(std::shared_ptr<NodeAction>(new NodeAction("VertexShowSelection",true,tr("Show Selection"),tr("Vertices"))));
    actions_.push_back(std::shared_ptr<NodeAction>(new NodeAction("VertexColor",true,tr("Color"),tr("Vertices"))));

    actions_.push_back(std::shared_ptr<NodeAction>(new NodeAction("EdgeVisible",true,tr("Visible"),tr("Edges"))));
    actions_.push_back(std::shared_ptr<NodeAction>(new NodeAction("EdgeShowSelection",true,tr("Show Selection"),tr("Edges"))));
    actions_.push_back(std::shared_ptr<NodeAction>(new NodeAction("EdgeColor",true,tr("Color"),tr("Edges"))));

    actions_.push_back(std::shared_ptr<NodeAction>(new NodeAction("FaceVisible",true,tr("Visible"),tr("Faces"))));
    actions_.push_back(std::shared_ptr<NodeAction>(new NodeAction("FaceShowSelection",true,tr("Show Selection"),tr("Faces"))));
    actions_.push_back(std::shared_ptr<NodeAction>(new NodeAction("FaceColor",true,tr("Color"),tr("Faces"))));

}

bool Node::getNodeActionValue(const QString &action_name)
{
    if(action_name == "RenderSettingOverride")
    {
        return render_setting_overriding_;
    }
    else if(action_name == "VertexVisible")
    {
        return this->getRenderModes()->has_render_mode(RenderModes::VERTEX_DRAWING);
    }
    else if(action_name == "VertexShowSelection")
    {
        return this->getRenderModes()->has_render_mode(RenderModes::VERTEX_SELECTION);
    }
    else if(action_name == "VertexColor")
    {
        return this->getRenderModes()->has_render_mode(RenderModes::VERTEX_COLOR);
    }
    else if(action_name == "EdgeVisible")
    {
        return this->getRenderModes()->has_render_mode(RenderModes::EDGE_DRAWING);
    }
    else if(action_name == "EdgeShowSelection")
    {
        return this->getRenderModes()->has_render_mode(RenderModes::EDGE_SELECTION);
    }
    else if(action_name == "EdgeColor")
    {
        return this->getRenderModes()->has_render_mode(RenderModes::EDGE_COLOR);
    }
    else if(action_name == "FaceVisible")
    {
        return this->getRenderModes()->has_render_mode(RenderModes::FACE_DRAWING);
    }
    else if(action_name == "FaceShowSelection")
    {
        return this->getRenderModes()->has_render_mode(RenderModes::FACE_SELECTION);
    }
    else if(action_name == "FaceColor")
    {
        return this->getRenderModes()->has_render_mode(RenderModes::FACE_COLOR);
    }
    return false;
}

void Node::addNodeAction(const std::shared_ptr<NodeAction> &action)
{
    actions_.push_back(action);
}

int Node::getNodeActionsNumber() const
{
    return actions_.size();
}

NodeAction *Node::getNodeAction(int i)
{
    return actions_[i].get();
}

void Node::slotAction(const QString &action_name)
{

}

void Node::slotCheckableAction(const QString &action_name, bool checked)
{
    if(action_name == "RenderSettingOverride")
    {
        render_setting_overriding_ = checked;
        this->nodeUpdated();
    }
    else if(action_name == "VertexVisible")
    {
        this->getRenderModes()->set_render_mode(RenderModes::VERTEX_DRAWING,checked);
        this->nodeUpdated();
    }
    else if(action_name == "VertexShowSelection")
    {
        this->getRenderModes()->set_render_mode(RenderModes::VERTEX_SELECTION,checked);
        this->nodeUpdated();
    }
    else if(action_name == "VertexColor")
    {
        this->getRenderModes()->set_render_mode(RenderModes::VERTEX_COLOR,checked);
        this->nodeUpdated();
    }
    else if(action_name == "EdgeVisible")
    {
        this->getRenderModes()->set_render_mode(RenderModes::EDGE_DRAWING,checked);
        this->nodeUpdated();
    }
    else if(action_name == "EdgeShowSelection")
    {
        this->getRenderModes()->set_render_mode(RenderModes::EDGE_SELECTION,checked);
        this->nodeUpdated();
    }
    else if(action_name == "EdgeColor")
    {
        this->getRenderModes()->set_render_mode(RenderModes::EDGE_COLOR,checked);
        this->nodeUpdated();
    }
    else if(action_name == "FaceVisible")
    {
        this->getRenderModes()->set_render_mode(RenderModes::FACE_DRAWING,checked);
        this->nodeUpdated();
    }
    else if(action_name == "FaceShowSelection")
    {
        this->getRenderModes()->set_render_mode(RenderModes::FACE_SELECTION,checked);
        this->nodeUpdated();
    }
    else if(action_name == "FaceColor")
    {
        this->getRenderModes()->set_render_mode(RenderModes::FACE_COLOR,checked);
        this->nodeUpdated();
    }

}

}}

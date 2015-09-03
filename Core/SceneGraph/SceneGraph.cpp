#include "NodeType.h"
#include "SceneGraph.h"
#include "Camera.h"
#include "Lights.h"
#include "Visitor.h"
#include "EventHandler/TrackBallManipulator.h"
#include "Selection/RectSelection.h"
#include "Decorations/DecorationBase.h"
#include "NodeFactory.h"
#include "../DataStructures/AABB.h"
#include <algorithm>
#include "GLStates.h"
#include "Shader.h"
namespace GCL{ namespace SceneGraph {

SceneGraphNode::SceneGraphNode()
    : Node(0),camera_(new Camera()),gl_states_(new GLStates()),
      lights_(new Lights()), node_factory_(new NodeFactory()),shader_(new Shader())
{
    manipulator_ = std::shared_ptr<Manipulator>(new TrackBallManipulator(this));
    event_handlers_.push_back(std::shared_ptr<EventHandler>(new RectSelection(this)));
    lights_->setDefaultLights();
    this->setName("Root");

}

SceneGraphNode::~SceneGraphNode()
{
//    DebugLog<<"~SceneGraphNode()"<<std::endl;
}

Node *SceneGraphNode::getScenegraph()
{
    return this;
}

int SceneGraphNode::getNodeType() const
{
    return SCENEGRAPH_ROOT_NODE;
}

QString SceneGraphNode::getNodeTypeName() const
{
    return "SceneGraph";
}

////visitor
void SceneGraphNode::accept(Visitor & _visitor){
    if(_visitor.enter(this)) {
        for(int i=0; i<getNumberOfChildren(); ++i) {
            getChild(i)->accept(_visitor);
        }
    }
    _visitor.leave(this);
}


void SceneGraphNode::lock()
{
    mutex_.lock();
}

void SceneGraphNode::unlock()
{
    mutex_.unlock();
}

void SceneGraphNode::setManipulator(const std::shared_ptr<Manipulator> &_manipulator)
{
    bool is_show_arcball = manipulator_->is_show_arcball();
    manipulator_->set_show_arcball(false);
    manipulator_ = _manipulator;
    manipulator_->set_show_arcball(is_show_arcball);
}

void SceneGraphNode::addEventHandler(const std::shared_ptr<EventHandler> &_handler)
{
    event_handlers_.push_back(_handler);
}

std::shared_ptr<EventHandler> SceneGraphNode::getEventHandlerByName(const QString &name)
{
    foreach (const std::shared_ptr<EventHandler>& handler , event_handlers_)
    {
        if(QString(handler->getEventHandlerName()) == name)
        {
            return handler;
        }
    }
    return std::shared_ptr<EventHandler>();
}


void SceneGraphNode::removeEventHandlerByName(const QString &name)
{
    // A little stupid mothod for removing elements...
    for(int i=0; i < event_handlers_.size();)
    {
        if(QString(event_handlers_[i]->getEventHandlerName()) == name)
        {
            event_handlers_.erase(event_handlers_.begin() + i);
        }
        else
        {
            i++;
        }
    }
}

void SceneGraphNode::removeEventHandlerByCategory(const QString &category)
{
    // A little stupid mothod for removing elements...
    for(int i=0; i < event_handlers_.size();)
    {
        if(QString(event_handlers_[i]->getCategory()) == category)
        {
            event_handlers_.erase(event_handlers_.begin() + i);
        }
        else
        {
            i++;
        }
    }
}

void SceneGraphNode::addDecoration(const std::shared_ptr<Decoration::DecorationBase> &_render_element)
{
    decorations_.push_back(_render_element);
}

std::shared_ptr<Decoration::DecorationBase> SceneGraphNode::getDecorationByName(const QString &name)
{
    foreach (const std::shared_ptr<Decoration::DecorationBase>& element , decorations_)
    {
        if(element->getName() == name)
        {
            return element;
        }
    }
    return std::shared_ptr<Decoration::DecorationBase>();
}

void SceneGraphNode::removeDecorationByName(const QString &name)
{
    // A little stupid mothod for removing elements...
    for(int i=0; i < decorations_.size();)
    {
        if(decorations_[i]->getName() == name)
        {
            decorations_.erase(decorations_.begin() + i);
        }
        else
        {
            i++;
        }
    }
}

void SceneGraphNode::renderDecorations(bool with_camera)
{
    foreach(const std::shared_ptr<Decoration::DecorationBase>& element, decorations_)
    {
        if(element->is_with_camera() == with_camera)
        {
            element->render();
        }

    }
}

void SceneGraphNode::resize(int w, int h)
{
    camera_->set_viewport(w,h);
    manipulator_->resize(w,h);
    foreach(const std::shared_ptr<EventHandler>& handler, event_handlers_)
    {
        if(handler->isEnable())
        {
            if(handler->resize(w,h))
            {
                return;
            }
        }
    }

}

void SceneGraphNode::mouseMove(int posX, int posY)
{
    if(manipulator_->isEnable())
    {
        manipulator_->mouseMove(posX,posY);
    }
    foreach(const std::shared_ptr<EventHandler>& handler, event_handlers_)
    {
        if(handler->isEnable())
        {
            if(handler->mouseMove(posX,posY))
            {
                return;
            }
        }
    }
}

void SceneGraphNode::mousePress(int posX, int posY, unsigned int button, unsigned int modifier)
{
    if(manipulator_->isEnable())
    {
        manipulator_->mousePress(posX,posY,button,modifier);
    }
    foreach(const std::shared_ptr<EventHandler>& handler, event_handlers_)
    {
        if(handler->isEnable())
        {
            if(handler->mousePress(posX,posY,button,modifier))
            {
                return;
            }
        }
    }
}

void SceneGraphNode::mouseRelease(int posX, int posY, unsigned int button, unsigned int modifier)
{
    if(manipulator_->isEnable())
    {
        manipulator_->mouseRelease(posX,posY,button,modifier);
    }
    foreach(const std::shared_ptr<EventHandler>& handler, event_handlers_)
    {
        if(handler->isEnable())
        {
            if(handler->mouseRelease(posX,posY,button,modifier))
            {
                return;
            }
        }
    }
}

void SceneGraphNode::keyUp(const unsigned int &key, unsigned int modifier)
{
    if(manipulator_->isEnable())
    {
        manipulator_->keyUp(key,modifier);
    }
    foreach(const std::shared_ptr<EventHandler>& handler, event_handlers_)
    {
        if(handler->isEnable())
        {
            if(handler->keyUp(key,modifier))
            {
                return;
            }
        }
    }
}

void SceneGraphNode::keyDown(const unsigned int &key, unsigned int modifier)
{
    if(manipulator_->isEnable())
    {
        manipulator_->keyDown(key,modifier);
    }
    foreach(const std::shared_ptr<EventHandler>& handler, event_handlers_)
    {
        if(handler->isEnable())
        {
            if(handler->keyDown(key,modifier))
            {
                return;
            }
        }
    }
}

void SceneGraphNode::wheel(const float &delta)
{
    if(manipulator_->isEnable())
    {
        manipulator_->wheel(delta);
    }
    foreach(const std::shared_ptr<EventHandler>& handler, event_handlers_)
    {
        if(handler->isEnable())
        {
            if(handler->wheel(delta))
            {
                return;
            }
        }
    }
}

void SceneGraphNode::focusOnNode(Node *_node)
{
    if(!_node) return;
    const AABB* aabb = _node->getAABB();
    if(aabb->isEmpty()) return;
    manipulator_->focus(aabb->getCenter(),aabb->getRadius() * 3);
}

void SceneGraphNode::nodeUpdated(int dirtyType)
{
    emit onUpdate();
}

}}

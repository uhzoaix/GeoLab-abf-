#include "Manipulator.h"
#include "../SceneGraph.h"
#include "../Decorations/Arcball.h"
namespace GCL { namespace SceneGraph {

Manipulator::Manipulator(SceneGraphNode *_scenegraph)
    : scenegraph_(_scenegraph),show_arcball_(false),arcball_(new Decoration::Arcball())
{
    arcball_->setRadius(0);
}

Manipulator::~Manipulator()
{
}

void Manipulator::focus(const Vec3 &center, const Scalar &radius)
{
}

void Manipulator::set_show_arcball(bool t)
{
    if(t == show_arcball_) return;
    if(t)
    {
        getSceneGraph()->addDecoration(arcball_);
    }
    else
    {
        getSceneGraph()->removeDecorationByName(arcball_->getName());
    }
    show_arcball_ = t;
}

}}


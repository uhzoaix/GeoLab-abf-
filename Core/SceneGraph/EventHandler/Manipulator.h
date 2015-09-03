#ifndef GCL_MANIPULATOR_H
#define GCL_MANIPULATOR_H
#include "gcl_global.h"
#include "EventHandler.h"
#include "Core/Math/MathDefines.h"
#include <memory>
namespace GCL {
namespace Decoration {
class Arcball;
}
namespace SceneGraph {
/*
* Manipulator is a sub class of EventHandler. It provides a basic interface of manipulator,
* e.g. TrackballManipulator. To manipulate a scene is just to modify the camera of the scene.
*/
class SceneGraphNode;
class GCL_CORE_SHARED_EXPORT Manipulator : public EventHandler
{
public:
    Manipulator(SceneGraphNode* _scenegraph);
    virtual ~Manipulator();

    EventHandlerType getType() const {return EventHandler::Manipulator;}
    virtual void focus(const Vec3& center, const Scalar& radius);
    void set_show_arcball(bool t);
    bool is_show_arcball() const {return show_arcball_;}
protected:
    SceneGraphNode* getSceneGraph() {return scenegraph_;}
    std::shared_ptr<Decoration::Arcball> arcball_;
private:
    SceneGraphNode* scenegraph_;
    bool show_arcball_;
};
}}
#endif // MANIPULATOR_H

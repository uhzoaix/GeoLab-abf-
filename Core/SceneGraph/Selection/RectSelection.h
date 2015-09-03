#ifndef GCL_SECNEGRAPH_RECTSELECTION_H
#define GCL_SECNEGRAPH_RECTSELECTION_H
#include "gcl_global.h"
#include "AbstractSelection.h"
#include <memory>
namespace GCL {

namespace Decoration {
class Rect;
}
namespace SceneGraph {

class GCL_CORE_SHARED_EXPORT RectSelection : public AbstractSelection
{
public:
    explicit RectSelection(SceneGraphNode* _scenegraph);
public:
    int selection_start(int _x, int _y);
    int selection_moving(int _x, int _y);
    int selection_stop(int _x, int _y);

    bool isInsideVertex(const Vec3 &_a, const Vec3 &_na) const;

    const char* getEventHandlerName() const {return "RectSelection";}

private:
    int start_x_;
    int start_y_;
    int stop_x_;
    int stop_y_;
    std::shared_ptr<Decoration::Rect> rect_;
};
}}
#endif // RECTSELECTION_H

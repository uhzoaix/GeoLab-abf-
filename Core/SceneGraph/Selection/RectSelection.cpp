#include "RectSelection.h"
#include "../Decorations/Rect.h"
#include "../SceneGraph.h"
#include "../Camera.h"
#include <qgl.h>
#include "GL/glu.h"
namespace GCL { namespace SceneGraph {



RectSelection::RectSelection(SceneGraphNode *_scenegraph)
    :AbstractSelection(_scenegraph),rect_(new Decoration::Rect())
{
    rect_->setName("RectSelection:Rect");
}

int RectSelection::selection_start(int _x, int _y)
{

    Vec4i viewport = getScenegraph()->getCamera()->get_viewport();
    start_x_ = _x;
    start_y_ = _y;

    // Decoration: Rect
    getScenegraph()->addDecoration(rect_);
    rect_->setStartPoint((float)_x /(float)viewport[2],(float)_y / (float) viewport[3]);
    return  GCL_TRUE;
}

int RectSelection::selection_moving(int _x, int _y)
{
    Vec4i viewport = getScenegraph()->getCamera()->get_viewport();
    // Decoration: Rect
    rect_->setEndPoint((float)_x /(float)viewport[2],(float)_y / (float) viewport[3]);

    return GCL_TRUE;
}

int RectSelection::selection_stop(int _x, int _y)
{
    stop_x_ = _x;
    stop_y_ = _y;
    rect_->setStartPoint(0,0);
    getScenegraph()->removeDecorationByName(rect_->getName());
    startSelection();

    return GCL_TRUE;
}

bool RectSelection::isInsideVertex(const Vec3 &_a, const Vec3 &_na) const
{
    // get windows corrdinates
    double winx, winy, winz;
    gluProject(_a[0],_a[1],_a[2],
            getScenegraph()->getCamera()->get_model_matrix().data(),
            getScenegraph()->getCamera()->get_project_matrix().data(),
            getScenegraph()->getCamera()->get_viewport().data(),
            &winx,&winy,&winz);
    Vec3 _s(winx,winy,winz);
    int min_x = start_x_ < stop_x_ ? start_x_ : stop_x_;
    int max_x = start_x_ > stop_x_ ? start_x_ : stop_x_;
    int min_y = start_y_ < stop_y_ ? start_y_ : stop_y_;
    int max_y = start_y_ > stop_y_ ? start_y_ : stop_y_;
    if(winx>=min_x && winx <= max_x && winy >= min_y && winy <= max_y)
    {
        if(!checkPreFilter(_a,_s,_na)) return false;
        return true;
    }
    return false;
}


}}

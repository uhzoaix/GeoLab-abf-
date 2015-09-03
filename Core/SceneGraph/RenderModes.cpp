#include "RenderModes.h"
#include "Node.h"
namespace GCL{ namespace SceneGraph {

RenderModes::RenderModes()
    :node_(NULL),render_mode_(0)
{

}

RenderModes::RenderModes(Node *node)
    :node_(node),render_mode_(  FACE_DRAWING  | FACE_SELECTION)
{

}

void RenderModes::reset_render_mode(int _render_mode)
{
    set_render_mode(_render_mode,false);
}

void RenderModes::set_render_mode(int _render_mode, bool _enable)
{
    int tmp_mode = 0;
    if(_enable)
    {
        tmp_mode = _render_mode | render_mode_;
    }
    else
    {
        tmp_mode = render_mode_ - (_render_mode & render_mode_);
    }
    if(tmp_mode != render_mode_)
    {
        render_mode_ = tmp_mode;
        if(node_)
        {
            node_->nodeUpdated();
        }
    }
}

void RenderModes::reset_default_render_mode()
{
    set_render_mode(FACE_DRAWING | SMOOTH_SHADING);
}

bool RenderModes::operator==(const RenderModes &other)
{
    return (render_mode_ == other.render_mode_);
}

bool RenderModes::operator!=(const RenderModes &other)
{
    return !(render_mode_ == other.render_mode_);
}




}}

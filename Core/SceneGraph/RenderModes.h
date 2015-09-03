#ifndef GCL_RENDERMODES_H
#define GCL_RENDERMODES_H
#include "Core/gcl_global.h"

namespace GCL{ namespace SceneGraph {
/**


*/
class Node;
class GCL_CORE_SHARED_EXPORT RenderModes
{
public:
    enum RenderMode
    {
        // Drawing Mode
        VERTEX_DRAWING = 1,
        EDGE_DRAWING = 1 << 1,
        FACE_DRAWING = 1 << 2,

        // Shading Mode
        SMOOTH_SHADING = 1 << 3,

        // Color Mode
        VERTEX_COLOR = 1 << 4,
        EDGE_COLOR = 1 << 5,
        FACE_COLOR = 1 << 6,

        // Selection Mode
        VERTEX_SELECTION = 1 << 7,
        EDGE_SELECTION = 1 << 8,
        FACE_SELECTION = 1 << 9,

        // TEXTURE
        TEXTURE = 1 << 10,

    };
    RenderModes();
    RenderModes(Node* node);
    bool has_render_mode(int _render_mode) const {return (render_mode_ & _render_mode);}
    void reset_render_mode(int _render_mode);
    void set_render_mode(int _render_mode, bool _enable = true);

    int get_render_mode() const {return render_mode_;}
    void set_render_mode_directly(int _render_mode) {render_mode_ = _render_mode;}
    void reset_default_render_mode();

    void setNotifiyNode(Node* node) {node_ = node;}

    /**
      Returns true if all the modes are the same.
    */
    bool operator==(const RenderModes &other);

    /**
      Same as !(a == b). See == operator.
    */
    bool operator!=(const RenderModes &other);
private:
    int render_mode_;
    Node *node_;

};
}}
#endif // RENDERMODE_H

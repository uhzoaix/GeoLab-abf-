#ifndef GEOMETRYNODE_H
#define GEOMETRYNODE_H

#include "Node.h"
#include <memory>
#include "../gclnamespace.h"
namespace GCL{
class GeometryBase;

namespace SceneGraph {
class RenderModes;
class Material;
class AbstractSelection;

class GCL_CORE_SHARED_EXPORT SelectionMode{

public:

  SelectionMode(){
    v=false; e=false; f=false;
  }

  SelectionMode( bool _v, bool _e, bool _f ){
    v=_v; e=_e; f=_f;
  }

  bool vertex() const {return v;}
  void setVertex( bool b){ v=b;}

  bool edge() const {return e;}
  void setEdge( bool b){ e=b;}

  bool face() const {return f;}
  void setFace( bool b){ f=b;}

  void reset(){
    v=false; e=false; f=false;
  }

private:
  bool v;
  bool e;
  bool f;
};

class GCL_CORE_SHARED_EXPORT GeometryNode : public Node
{
    Q_OBJECT
public:


    explicit GeometryNode(Node* parent = 0);
    virtual ~GeometryNode();

    int getNodeType() const;


    virtual QString getNodeTypeName() const {return tr("GeometryNode");}

    virtual void recomputeAABB();
    /**
        If you want to do your own render, overrid this function;

        The procedure to draw is the following:


          - call setActiveModes, this will use lastActiveModes_ and change it according to render modes while also setting the dirty flags on arrays that
            need to be changed depending on any changes to render modes

          - call fill_vbo_data()

          - call renderPrologue()

          - call render_vbo_data()

          - call nodeSpecificRendering()

          - call renderCurtainCall()

    */
    virtual void render(RenderModes* pRenderModes);

    /**
      Sets up some OpenGL state variables using the node's material and renders the DrawData contained in this node (drawData_).
      If you want to do your own OpenGl state before rendering, override this function
    **/
    virtual void renderPrologue();



    /**
      If you want do render additional object beyond what is in DrawData (e.g. control points for splines), override this function.
      It is always called at the end of render, and the GeometryNode implementation does nothing.
    */
    virtual void nodeSpecificRendering();

    /**
        When rendering is over, it is need to clear some openGL states: like   glDisable(GL_COLOR_MATERIAL), or shader_->release()
    */
    virtual void renderCurtainCall();

    virtual GeometryBase* getGeometry() = 0;


    virtual void accept(Visitor &_visitor);


    Material* getMaterial() {return material_.get();}


    void nodeUpdated(int dirtype);
    void updateGeometryNode(int dirtyType) {dirtyType_ = dirtyType;}

public:
    void selectWithSelector(AbstractSelection *_selector);
protected:

    void setActiveModes(RenderModes* pRenderModes);

    virtual void fill_vbo_data(RenderModes* pRenderModes);
    virtual void fill_vbo_geometry_data(RenderModes * pRenderModes);
    virtual void fill_vbo_topology_data(RenderModes * pRenderModes);
    virtual void fill_vbo_color_data(RenderModes* pRenderModes);


    virtual void render_vbo_data(RenderModes* pRenderModes);
    virtual void render_vbo_vertex_data(RenderModes* pRenderModes);
    virtual void render_vbo_edge_data(RenderModes* pRenderModes);
    virtual void render_vbo_face_data(RenderModes* pRenderModes);

    void release_all_vbo_buffer();
    void release_vbo_array_buffer(unsigned int &buffer);
    //    virtual void render_vbo_quadrangle_data();
    //    virtual void render_vbo_polygon_data();

    unsigned int vbo_vertex_buffer_;
    unsigned int vbo_vertex_normal_buffer_;
    unsigned int vbo_vertex_color_buffer_;
    unsigned int vbo_edge_color_buffer_;
    unsigned int vbo_edge_indices_buffer_;
    unsigned int vbo_triangle_color_buffer_;
    unsigned int vbo_triangle_indices_buffer_;

    unsigned int vbo_vertex_size_;
    unsigned int vbo_edge_size_;
    unsigned int vbo_triangle_size_;

    int dirtyType_;

    void setSelectRenderModes();
    inline void setSelectionMode( SelectionMode m ) { selectionMode_ = m; }
    inline const SelectionMode& getSelectionMode() const { return selectionMode_; }
private:
    SelectionMode selectionMode_;
    std::shared_ptr<Material> material_;
    std::shared_ptr<RenderModes> lastActiveModes_;

};
}}
#endif // GEOMETRYNODE_H

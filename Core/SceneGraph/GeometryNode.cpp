#include "NodeType.h"
#include "GeometryNode.h"
#include "RenderModes.h"
#ifndef _APPLE
#include <GL/glew.h>
#endif
#include <qgl.h>
#include <GL/glu.h>
//#include<GL/gl.h>
//#include<GL/glu.h>

#include "../DataStructures/GeometryBase.h"
#include "Visitor.h"
#include "Material.h"
#include "RenderModes.h"
#include "Selection/AbstractSelection.h"
#include "../DataStructures/AABB.h"
namespace GCL{ namespace SceneGraph {

GeometryNode::GeometryNode(Node *parent)
    :Node(parent),material_(new Material()),
      vbo_vertex_buffer_(0),vbo_vertex_color_buffer_(0),vbo_vertex_normal_buffer_(0),
      vbo_edge_color_buffer_(0),vbo_edge_indices_buffer_(0),
      vbo_triangle_color_buffer_(0),
      vbo_triangle_indices_buffer_(0),
      vbo_vertex_size_(0),
      vbo_edge_size_(0),
      vbo_triangle_size_(0),
      lastActiveModes_(new RenderModes())
{

    dirtyType_ = Totally_Dirty;
    setSelectionMode(SelectionMode(true,true,true));

    //    this->setName("GeometryNode");
}

GeometryNode::~GeometryNode()
{
    release_all_vbo_buffer();
}

int GeometryNode::getNodeType() const
{
    return Geometry_Node;
}


void GeometryNode::recomputeAABB()
{
    aabb_->setEmpty();
    for(size_t i=0; i < getGeometry()->getPointsNumber(); i++)
    {
        aabb_->extend(getGeometry()->getVertexPosition(i));
    }
    aabbIsDirty_ = false;
}


void GeometryNode::render(RenderModes *pRenderModes)
{


    if(getGeometry() == NULL) return;

    GLenum err = glewInit();
    if( GLEW_OK != err )
    {
        std::cout <<"Error initializing GLEW: " << glewGetErrorString(err) << std::endl;
    }
    setActiveModes(pRenderModes);

    fill_vbo_data(pRenderModes);

    renderPrologue();

    render_vbo_data(pRenderModes);

    nodeSpecificRendering();

    renderCurtainCall();
}

void GeometryNode::renderPrologue()
{
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, getMaterial()->getSpecularity4fv());
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, getMaterial()->getShininess());
    glLineWidth(getMaterial()->getEdgeWidth());
    glPointSize(getMaterial()->getVertexRadius());
}

void GeometryNode::nodeSpecificRendering()
{

}

void GeometryNode::renderCurtainCall()
{
    glDisable(GL_COLOR_MATERIAL);

}

void GeometryNode::accept(Visitor &_visitor)
{
    if(_visitor.enter(this))
    {
        _visitor.leave(this);
    }

    for(int i=0; i<getNumberOfChildren(); ++i)
    {
        getChild(i)->accept(_visitor);
    }

}

void GeometryNode::nodeUpdated(int dirtype)
{
    dirtyType_ = dirtype;
    Node::nodeUpdated(dirtype);
}

void GeometryNode::selectWithSelector(AbstractSelection *_selector)
{
    bool finished=false;
    Vec3 p1, p2, p3;
    Vec3 n1, n2, n3;

    // Note: This code can be optimized, to reduce the number of computation steps
    // linear in the amount of entities, at the cost of readability and total code size
    if(selectionMode_.face()){


        for (size_t i=0; i < getGeometry()->getFacesNumber(); i++)
        {
            GeometryBase::TriIndices t = getGeometry()->getFaceTriIndices(i);
            p1 = getGeometry()->getVertexPosition(t[0]);
            n1 = getGeometry()->getVertexNormal(t[0]);
            p2 = getGeometry()->getVertexPosition(t[1]);
            n2 = getGeometry()->getVertexNormal(t[1]);
            p3 = getGeometry()->getVertexPosition(t[2]);
            n3 = getGeometry()->getVertexNormal(t[2]);

            if(!_selector->objectMode())
            {
                bool sf = getGeometry()->isFaceSelected(i);
                getGeometry()->setFaceSelected(i,_selector->selectFace(sf,finished,p1, p2, p3, n1, n2, n3));
            }
            else
            {
                setSelected(_selector->selectFace(selected(),finished,p1, p2, p3, n1, n2, n3));
                if(finished) return;
            }
        }
    }

    //edges
    if(selectionMode_.edge()){

        for (size_t i=0; i < getGeometry()->getEdgesNumber(); i++)
        {
            GeometryBase::EdgeIndices t = getGeometry()->getEdgeVertexIndices(i);
            p1 = getGeometry()->getVertexPosition(t[0]);
            n1 = getGeometry()->getVertexNormal(t[0]);
            p2 = getGeometry()->getVertexPosition(t[1]);
            n2 = getGeometry()->getVertexNormal(t[1]);

            if(!_selector->objectMode())
            {
                bool se = getGeometry()->isEdgeSelected(i);
                getGeometry()->setEdgeSelected(i,_selector->selectEdge(se,finished,p1, p2, n1, n2));
            }
            else
            {
                setSelected(_selector->selectEdge(selected(),finished,p1, p2, n1, n2 ));
                if(finished) return;
            }
        }
    }

    //vertices
    if(selectionMode_.vertex() || _selector->objectMode() )
    {
        for (size_t i=0; i < getGeometry()->getPointsNumber(); i++)
        {
            p1 = getGeometry()->getVertexPosition(i);
            n1 = getGeometry()->getVertexNormal(i);

            if(!_selector->objectMode())
            {
                bool sv =getGeometry()->isVertexSelected(i);
                getGeometry()->setVertexSelected(i,_selector->selectVertex(sv,finished,p1, n1));
                //                if(getGeometry()->isVertexSelected(i))
                //                {
                //                    DebugLog<<i<<std::endl;
                //                }
            }
            else
            {
                setSelected(_selector->selectVertex(selected(),finished,p1, n1 ));
                if(finished) return;
            }
        }
    }

    if(!_selector->objectMode())
        setSelectRenderModes();

    nodeUpdated(Color_Dirty);
}

void GeometryNode::setActiveModes(RenderModes *pRenderModes)
{
    if((*pRenderModes) != (*lastActiveModes_))
    {
        dirtyType_ = dirtyType_ | Color_Dirty;
    }
    if(!(lastActiveModes_->has_render_mode(RenderModes::SMOOTH_SHADING) == pRenderModes->has_render_mode(RenderModes::SMOOTH_SHADING)))
    {
        dirtyType_ = dirtyType_ | Totally_Dirty;
    }
    if(pRenderModes->has_render_mode(RenderModes::SMOOTH_SHADING) && dirtyType_ | Topology_Dirty)
    {
        dirtyType_ = dirtyType_ | Geometry_Dirty;
    }
    lastActiveModes_->set_render_mode_directly(pRenderModes->get_render_mode());
}

void GeometryNode::fill_vbo_data(RenderModes *pRenderModes)
{
    //    GLenum err = glewInit();
    //    if( GLEW_OK != err )
    //    {
    //        std::cout <<"Error initializing GLEW: " << glewGetErrorString(err) << std::endl;
    //    }
    if(dirtyType_ & Geometry_Dirty)
    {
        //        DebugLog<<"Geometry_Dirty"<<std::endl;
        fill_vbo_geometry_data(pRenderModes);
    }
    if(dirtyType_ & Topology_Dirty)
    {
        //        DebugLog<<"Topology_Dirty"<<std::endl;

        fill_vbo_topology_data(pRenderModes);
    }
    if(dirtyType_ & Color_Dirty)
    {
        fill_vbo_color_data(pRenderModes);
    }
    // clear dirty Type
    dirtyType_ = 0;
}

void GeometryNode::fill_vbo_geometry_data(RenderModes *pRenderModes)
{
    //    DebugLog<<"fill_vbo_geometry_data()"<<std::endl;
    release_vbo_array_buffer(vbo_vertex_buffer_);
    release_vbo_array_buffer(vbo_vertex_normal_buffer_);
    std::vector<float> vertices;
    std::vector<float> normals;

    if(pRenderModes->has_render_mode(RenderModes::SMOOTH_SHADING))
    {
        for(size_t i=0; i < getGeometry()->getPointsNumber(); i++)
        {
            Vec3 v = getGeometry()->getVertexPosition(i);
            Vec3 vn = getGeometry()->getVertexNormal(i);
            for(int j=0; j < 3; j++)
            {
                vertices.push_back(v[j]);
                normals.push_back(vn[j]);
            }
        }
    }
    else
    {
        for(size_t i=0; i < getGeometry()->getFacesNumber(); i++)
        {
            GeometryBase::TriIndices indices = getGeometry()->getFaceTriIndices(i);
            Vec3 v[3];
            for(int j=0; j < 3; j++)
            {
                v[j] = getGeometry()->getVertexPosition(indices[j]);
            }
            Vec3 vn = (v[1] - v[0]) ^ (v[2] - v[1]);
            vn.Normalize();
            for(int j=0; j < 3; j++)
            {
                for(int k=0; k < 3; k++)
                {
                    vertices.push_back(v[j][k]);
                    normals.push_back(vn[k]);
                }
            }
        }
    }
    vbo_vertex_size_ = vertices.size();
    glGenBuffers(1,&vbo_vertex_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER,vbo_vertex_buffer_);
    glBufferData(GL_ARRAY_BUFFER,sizeof(float) * vbo_vertex_size_, vertices.data(),GL_STATIC_DRAW);
    glGenBuffers(1,&vbo_vertex_normal_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER,vbo_vertex_normal_buffer_);
    glBufferData(GL_ARRAY_BUFFER,sizeof(float) * vbo_vertex_size_, normals.data(),GL_STATIC_DRAW);
}

void GeometryNode::fill_vbo_topology_data(RenderModes *pRenderModes)
{
    release_vbo_array_buffer(vbo_edge_indices_buffer_);


    std::vector<unsigned int> edge_indices;


    if(pRenderModes->has_render_mode(RenderModes::SMOOTH_SHADING))
    {
        for(size_t i=0; i < getGeometry()->getEdgesNumber(); i++)
        {
            Vec2i v = getGeometry()->getEdgeVertexIndices(i);
            for(int j=0; j < 2; j++)
            {
                edge_indices.push_back(v[j]);
            }
        }
    }
    else
    {
        int count = 0;
        for(size_t i=0; i < getGeometry()->getFacesNumber(); i++)
        {
            for(int j=0;j < 3; j++)
            {
                edge_indices.push_back(count + j);
                edge_indices.push_back(count + (j+1) % 3);
            }
            count+=3;
        }
    }
    vbo_edge_size_ = edge_indices.size();
    glGenBuffers(1,&vbo_edge_indices_buffer_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vbo_edge_indices_buffer_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(unsigned int) * edge_indices.size(), edge_indices.data(),GL_STATIC_DRAW);


    if(!pRenderModes->has_render_mode(RenderModes::SMOOTH_SHADING)) return;
    std::vector<unsigned int> face_indices;

    release_vbo_array_buffer(vbo_triangle_indices_buffer_);

    for(size_t i=0; i < getGeometry()->getFacesNumber(); i++)
    {
        Vec3i v = getGeometry()->getFaceTriIndices(i);
        for(int j=0; j < 3; j++)
        {
            face_indices.push_back(v[j]);
        }
    }


    vbo_triangle_size_ = face_indices.size();


    glGenBuffers(1,&vbo_triangle_indices_buffer_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vbo_triangle_indices_buffer_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(unsigned int) * face_indices.size(), face_indices.data(),GL_STATIC_DRAW);

}

void GeometryNode::fill_vbo_color_data(RenderModes *pRenderModes)
{
    release_vbo_array_buffer(vbo_vertex_color_buffer_);
    release_vbo_array_buffer(vbo_edge_color_buffer_);
    release_vbo_array_buffer(vbo_triangle_color_buffer_);
    //    DebugLog<<"fill_vbo_color_data()"<<std::endl;

    if(pRenderModes->has_render_mode(RenderModes::VERTEX_DRAWING))
    {
        std::vector<float> vertex_colors;
        if(pRenderModes->has_render_mode(RenderModes::SMOOTH_SHADING))
        {
            for(size_t i=0; i <  getGeometry()->getPointsNumber(); i++)
            {
                Vec4 color;
                for(int j=0; j < 4; j++)
                {
                    color[j] = getMaterial()->getPointColor4fv()[j];
                }
                if(pRenderModes->has_render_mode(RenderModes::VERTEX_COLOR))
                {
                    color = getGeometry()->getVertexColor(i);
                }
                if(pRenderModes->has_render_mode(RenderModes::VERTEX_SELECTION))
                {
                    if(getGeometry()->isVertexSelected(i))
                    {
                        for(int j=0; j < 4; j++)
                        {
                            color[j] = getMaterial()->getSelectionColor4fv()[j];
                        }
                    }
                }
                for(int j=0; j < 4; j++)
                {
                    vertex_colors.push_back(color[j]);
                }
            }
        }
        else
        {
            for(size_t i=0; i < getGeometry()->getFacesNumber(); i++)
            {
                GeometryBase::TriIndices indices = getGeometry()->getFaceTriIndices(i);
                for(int k=0; k < 3; k++)
                {
                    int id = indices[k];
                    Vec4 color;
                    for(int j=0; j < 4; j++)
                    {
                        color[j] = getMaterial()->getPointColor4fv()[j];
                    }
                    if(pRenderModes->has_render_mode(RenderModes::VERTEX_COLOR))
                    {
                        color = getGeometry()->getVertexColor(id);
                    }
                    if(pRenderModes->has_render_mode(RenderModes::VERTEX_SELECTION))
                    {
                        if(getGeometry()->isVertexSelected(id))
                        {
                            for(int j=0; j < 4; j++)
                            {
                                color[j] = getMaterial()->getSelectionColor4fv()[j];
                            }
                        }
                    }
                    for(int j=0; j < 4; j++)
                    {
                        vertex_colors.push_back(color[j]);
                    }

                }
            }
        }
        glGenBuffers(1,&vbo_vertex_color_buffer_);
        glBindBuffer(GL_ARRAY_BUFFER,vbo_vertex_color_buffer_);
        glBufferData(GL_ARRAY_BUFFER,sizeof(float) * vertex_colors.size(), vertex_colors.data(),GL_STATIC_DRAW);

    }
    if(pRenderModes->has_render_mode(RenderModes::EDGE_DRAWING))
    {
        std::vector<float> edge_colors;
        if(pRenderModes->has_render_mode(RenderModes::SMOOTH_SHADING))
        {
            for(size_t i=0; i <  getGeometry()->getPointsNumber(); i++)
            {
                for(int j=0; j < 4; j++)
                {
                    edge_colors.push_back(getMaterial()->getEdgeColor4fv()[j]);
                }
            }
            for(size_t i=0; i < getGeometry()->getEdgesNumber(); i++)
            {
                if(pRenderModes->has_render_mode(RenderModes::EDGE_COLOR))
                {
                    Vec2i indices = getGeometry()->getEdgeVertexIndices(i);
                    Vec4  color = getGeometry()->getEdgeColor(i);
                    for(int j=0; j < 4; j++)
                    {
                        edge_colors[4 * indices[0] + j] = color[j];
                        edge_colors[4 * indices[1] + j] = color[j];
                    }
                }
                if(pRenderModes->has_render_mode(RenderModes::EDGE_SELECTION))
                {
                    if(getGeometry()->isEdgeSelected(i))
                    {
                        Vec2i indices = getGeometry()->getEdgeVertexIndices(i);
                        for(int j=0; j < 4; j++)
                        {
                            edge_colors[4 * indices[0] + j] = getMaterial()->getSelectionColor4fv()[j];
                            edge_colors[4 * indices[1] + j] = getMaterial()->getSelectionColor4fv()[j];
                        }
                    }
                }
            }
        }
        else
        {
            for(size_t i=0; i < getGeometry()->getFacesNumber(); i++)
            {
                GeometryBase::TriIndices indices = getGeometry()->getFaceTriIndices(i);
                for(int k=0; k < 3; k++)
                {
                    int id = indices[k];
                    Vec4 color;
                    for(int j=0; j < 4; j++)
                    {
                        color[j] = getMaterial()->getEdgeColor4fv()[j];
                    }
                    if(pRenderModes->has_render_mode(RenderModes::EDGE_SELECTION))
                    {
                        if(getGeometry()->isVertexSelected(id))
                        {
                            for(int j=0; j < 4; j++)
                            {
                                color[j] = getMaterial()->getSelectionColor4fv()[j];
                            }
                        }
                    }
                    for(int j=0; j < 4; j++)
                    {
                        edge_colors.push_back(color[j]);
                    }

                }
            }
        }
        glGenBuffers(1,&vbo_edge_color_buffer_);
        glBindBuffer(GL_ARRAY_BUFFER,vbo_edge_color_buffer_);
        glBufferData(GL_ARRAY_BUFFER,sizeof(float) * edge_colors.size(), edge_colors.data(),GL_STATIC_DRAW);

    }
    if(pRenderModes->has_render_mode(RenderModes::FACE_DRAWING))
    {
        std::vector<float> face_colors;
        if(pRenderModes->has_render_mode(RenderModes::SMOOTH_SHADING))
        {
            for(size_t i=0; i <  getGeometry()->getPointsNumber(); i++)
            {
                for(int j=0; j < 4; j++)
                {
                    face_colors.push_back(getMaterial()->getFaceColor4fv()[j]);
                }
            }
            for(size_t i=0; i < getGeometry()->getFacesNumber(); i++)
            {
                if(pRenderModes->has_render_mode(RenderModes::FACE_COLOR))
                {
                    Vec3i indices = getGeometry()->getFaceTriIndices(i);
                    for(int j=0; j < 4; j++)
                    {
                        face_colors[4 * indices[0] + j] = getGeometry()->getVertexColor(indices[0])[j];
                        face_colors[4 * indices[1] + j] = getGeometry()->getVertexColor(indices[1])[j];
                        face_colors[4 * indices[2] + j] = getGeometry()->getVertexColor(indices[2])[j];
                    }
                }
                if(pRenderModes->has_render_mode(RenderModes::FACE_SELECTION))
                {
                    if(getGeometry()->isFaceSelected(i))
                    {

                        Vec3i indices = getGeometry()->getFaceTriIndices(i);
                        for(int j=0; j < 4; j++)
                        {
                            face_colors[4 * indices[0] + j] = getMaterial()->getSelectionColor4fv()[j];
                            face_colors[4 * indices[1] + j] = getMaterial()->getSelectionColor4fv()[j];
                            face_colors[4 * indices[2] + j] = getMaterial()->getSelectionColor4fv()[j];
                        }
                    }
                }

            }
        }
        else
        {
            for(size_t i = 0; i < getGeometry()->getFacesNumber(); i++)
            {
                GeometryBase::TriIndices indices = getGeometry()->getFaceTriIndices(i);
                for(int k=0; k < 3; k++)
                {
                    int id = indices[k];
                    Vec4 color;
                    for(int j=0; j < 4; j++)
                    {
                        color[j] = getMaterial()->getFaceColor4fv()[j];
                    }
                    if(pRenderModes->has_render_mode(RenderModes::FACE_COLOR))
                    {
                        color = getGeometry()->getVertexColor(id);
                    }
                    if(pRenderModes->has_render_mode(RenderModes::FACE_SELECTION))
                    {
                        if(getGeometry()->isFaceSelected(i))
                        {
                            for(int j=0; j < 4; j++)
                            {
                                color[j] = getMaterial()->getFaceColor4fv()[j];
                            }
                        }
                    }
                    for(int j=0; j < 4; j++)
                    {
                        face_colors.push_back(color[j]);
                    }

                }
            }
        }
        glGenBuffers(1,&vbo_triangle_color_buffer_);
        glBindBuffer(GL_ARRAY_BUFFER,vbo_triangle_color_buffer_);
        glBufferData(GL_ARRAY_BUFFER,sizeof(float) * face_colors.size(), face_colors.data(),GL_STATIC_DRAW);
    }

}

void GeometryNode::render_vbo_data(RenderModes *pRenderModes)
{
    if(pRenderModes->has_render_mode(RenderModes::VERTEX_DRAWING))
    {
        render_vbo_vertex_data(pRenderModes);
    }
    if(pRenderModes->has_render_mode(RenderModes::EDGE_DRAWING))
    {
        render_vbo_edge_data(pRenderModes);
    }
    if(pRenderModes->has_render_mode(RenderModes::FACE_DRAWING))
    {
        render_vbo_face_data(pRenderModes);
    }
}

void GeometryNode::render_vbo_vertex_data(RenderModes* pRenderModes)
{
    Q_UNUSED(pRenderModes)
    if(vbo_vertex_buffer_ == 0) return;
    if(vbo_vertex_normal_buffer_ == 0) return;
    if(vbo_vertex_color_buffer_ == 0) return;
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER,vbo_vertex_buffer_);
    glVertexPointer(3,GL_FLOAT,  sizeof(float)  * 3 , (void *)0);
    glBindBuffer(GL_ARRAY_BUFFER,vbo_vertex_normal_buffer_);
    glNormalPointer(GL_FLOAT,sizeof(float)  * 3 ,(void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex_color_buffer_);
    glColorPointer(4,GL_FLOAT,sizeof(float) * 4, (void *)0);
    glDrawArrays(GL_POINTS,0,vbo_vertex_size_ / 3);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

void GeometryNode::render_vbo_edge_data(RenderModes* pRenderModes)
{
    if(vbo_vertex_buffer_ == 0) return;
    if(vbo_edge_color_buffer_ == 0) return;
    if(vbo_edge_indices_buffer_ == 0) return;
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER,vbo_vertex_buffer_);
    glVertexPointer(3,GL_FLOAT,  sizeof(float)  * 3 , (void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_edge_color_buffer_);
    glColorPointer(4,GL_FLOAT,sizeof(float) * 4, (void *)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vbo_edge_indices_buffer_);
    glDrawElements(GL_LINES, vbo_edge_size_,GL_UNSIGNED_INT, (void *)0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    //    glBegin(GL_LINES);
    //    for(int i=0; i < getGeometry()->getEdgesNumber(); i++)
    //    {
    //        Vec2i v = getGeometry()->getEdgeVertexIndices(i);
    //        glColor4fv(getMaterial()->getEdgeColor4fv());
    //        glVertex3dv(getGeometry()->getVertexPosition(v[0]).data());
    //        glVertex3dv(getGeometry()->getVertexPosition(v[1]).data());
    //    }
    //    glEnd();




}

void GeometryNode::render_vbo_face_data(RenderModes *pRenderModes)
{
    if(vbo_vertex_buffer_ == 0) return;
    if(vbo_triangle_color_buffer_ == 0) return;
    if(vbo_vertex_normal_buffer_ == 0) return;
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER,vbo_vertex_buffer_);
    glVertexPointer(3,GL_FLOAT,  sizeof(float)  * 3 , (void *)0);
    glBindBuffer(GL_ARRAY_BUFFER,vbo_vertex_normal_buffer_);
    glNormalPointer(GL_FLOAT,sizeof(float)  * 3 ,(void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_color_buffer_);
    glColorPointer(4,GL_FLOAT,sizeof(float) * 4, (void *)0);
    if(pRenderModes->has_render_mode(RenderModes::SMOOTH_SHADING) && vbo_triangle_indices_buffer_)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vbo_triangle_indices_buffer_);
        glDrawElements(GL_TRIANGLES,3 * getGeometry()->getFacesNumber() ,GL_UNSIGNED_INT, (void *)0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    }
    else
    {
        glDrawArrays(GL_TRIANGLES,0,vbo_vertex_size_ / 3);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

void GeometryNode::release_all_vbo_buffer()
{
    release_vbo_array_buffer(vbo_vertex_buffer_);
    release_vbo_array_buffer(vbo_vertex_color_buffer_);
    release_vbo_array_buffer(vbo_vertex_normal_buffer_);
    release_vbo_array_buffer(vbo_edge_color_buffer_);
    release_vbo_array_buffer(vbo_edge_indices_buffer_);
    release_vbo_array_buffer(vbo_triangle_color_buffer_);
    release_vbo_array_buffer(vbo_triangle_indices_buffer_);
}


void GeometryNode::release_vbo_array_buffer(unsigned int &buffer)
{
    //    DebugLog<<"release_vbo_array_buffer"<<" "<<buffer<<std::endl;
    if(buffer == 0) return;
    //    glBindBuffer(GL_ARRAY_BUFFER,buffer);
    //    DebugLog<<"glBindBuffer"<<" "<<buffer<<std::endl;

    //    glBufferData(GL_ARRAY_BUFFER,0,NULL,GL_STATIC_DRAW);
    //    DebugLog<<"glBufferData"<<" "<<buffer<<std::endl;

    //    glBindBuffer(GL_ARRAY_BUFFER,0);
    //    DebugLog<<"glBindBuffer"<<" "<<buffer<<std::endl;

    glDeleteBuffers(1,&buffer);
    //    DebugLog<<"glDeleteBuffers"<<" "<<buffer<<std::endl<<std::endl;

    buffer = 0;
}

void GeometryNode::setSelectRenderModes()
{
    //    if(selectionMode_.face())
    //    {
    //        getRenderModes()->set_render_mode(RenderModes::FACE_DRAWING);
    //        getRenderModes()->set_render_mode(RenderModes::FACE_SELECTION);
    //    }
    //    if(selectionMode_.edge())
    //    {
    //        getRenderModes()->set_render_mode(RenderModes::EDGE_DRAWING);
    //        getRenderModes()->set_render_mode(RenderModes::EDGE_SELECTION);
    //    }
    //    if(selectionMode_.vertex())
    //    {
    //        //modes->setMode(DrawModes::VERTEX_DRAWING,false);
    //        //modes->setMode(DrawModes::VERTEX_SELECTION,false);
    //    }
}


}}

#ifndef GCL_TRANSFORMATIONNODE_H
#define GCL_TRANSFORMATIONNODE_H

#include "gcl_global.h"
#include "SceneGraph/Node.h"
#include "Math/MathDefines.h"
namespace GCL { namespace SceneGraph {
class GCL_CORE_SHARED_EXPORT TransformationNode : public Node
{
    Q_OBJECT
public:
    enum TransformRenderType
    {
        TR_NONE = 0,
        TR_MOVING = 1,
        TR_ROTATE = 2,
        TR_SCALE = 3
    };

    explicit TransformationNode(Node* parent = 0);
    ~TransformationNode();

    //  return the basic nodetype defined in NodeType.h
    int getNodeType() const;
    //  return the node type name : it can be diffrent with nodetype
    QString getNodeTypeName() const;
    ////visitor pattern
    virtual void accept(Visitor & _visitor);



public:  // basic properties
    Scalar getScale() const;
    void setScale(const Scalar &getScale);

    Vec3 getOffset() const;
    void setOffset(const Vec3 &getOffset);

    Quat getRotation() const;
    void setRotation(const Quat &getRotation);


public:

    Vec3 get_center_for_render() const;
    void set_center_for_render(const Vec3 &get_center_for_render);

    Scalar get_radius_for_render() const;
    void set_radius_for_render(const Scalar &get_radius_for_render);

    int get_render_type() const;
    void set_render_type(int get_render_type);


public: // render
    virtual void render_enter();
    virtual void render_leave();



//    virtual void render_transformer();
private:

    Scalar scale_;
    Vec3   offset_;
    Quat   rotation_;



    int current_axis_for_render_;
    int render_type_;


};
}}
#endif // TRANSFORMATIONNODE_H

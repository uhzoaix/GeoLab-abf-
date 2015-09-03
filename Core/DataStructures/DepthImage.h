#ifndef GCL_DEPTHIMAGE_H
#define GCL_DEPTHIMAGE_H
#include "Core/gcl_global.h"
#include "Core/Math/MathDefines.h"
#include "Core/DataStructures/GeometryBase.h"
#include <vector>
namespace GCL {
class GCL_CORE_SHARED_EXPORT DepthImage  : public GeometryBase
{
public:
    class ColorDepth
    {
    public:
        ColorDepth(): color_(0,0,0,1),depth_(0),selected_(false) {}
        ColorDepth(const Vec4& _color, const Scalar& _depth):color_(_color),depth_(_depth),selected_(false){}
        const Vec4& color() const {return color_;}
        Vec4& color() {return color_;}
        Scalar depth() const {return depth_;}
        Scalar& depth() {return depth_;}

        const Scalar& red() const {return color_[0];}
        const Scalar& green() const {return color_[1];}
        const Scalar& blue() const {return color_[2];}
        const Scalar& alpha() const {return color_[3];}
        Scalar& red()  {return color_[0];}
        Scalar& green()  {return color_[1];}
        Scalar& blue()  {return color_[2];}
        Scalar& alpha()  {return color_[3];}

        bool isSelected() const {return  selected_;}
        void setSelected(bool t) {selected_ = t;}
    private:
        Vec4 color_;
        Scalar depth_;
        bool selected_;
    };

    DepthImage();
    DepthImage(size_t w, size_t h);

    void resize(size_t w, size_t h);

    void normalize();
    const Vec4& color(unsigned int x, unsigned int y) const;
    Vec4& color(unsigned int x, unsigned int y);

    Scalar depth(unsigned int x,unsigned int y) const;
    Scalar& depth(unsigned int x,unsigned int y);

    const ColorDepth& at(unsigned int x,unsigned int y) const;
    ColorDepth& at(unsigned int x, unsigned int y);

    const ColorDepth& at(unsigned int num) const;
    ColorDepth& at(unsigned int num);
public:

    void setCenter(const Vec3& center);
    void setAxis(const Vec3& axis_x, const Vec3& axis_y);
    int  getWidth() const {return width_;}
    int getHeight() const {return height_;}

    Vec3 getCenter() const {return center_;}
    Vec3 getAxisX() const {return axis_x_;}
    Vec3 getAxisY() const {return axis_y_;}
    Vec3 getAxisNormal() const {return (axis_x_ ^ axis_y_).normalize();}
public:
    unsigned int getPointsNumber() const { return width_ * height_;}
    Vec3 getVertexPosition(unsigned int i) const;
    void setVertexPosition(unsigned int i, const Vec3& pos) {}
    Vec3 getVertexNormal(unsigned int i) const;
    void setVertexNormal(unsigned int i, const Vec3& nor) {}
    Vec4 getVertexColor(unsigned int i) const ;
    void setVertexColor(unsigned int i, const Vec4& color);
    bool isVertexSelected(unsigned int i) const ;
    void setVertexSelected(unsigned int i, bool isSelected) ;

    unsigned int getEdgesNumber() const { return 0;}
    EdgeIndices getEdgeVertexIndices(unsigned int i) const {return EdgeIndices();}
    Vec4 getEdgeColor(unsigned int i) const {return Vec4();}
    void setEdgeColor(unsigned int i, const Vec4& color) {}
    bool isEdgeSelected(unsigned int i) const {return false;}
    void setEdgeSelected(unsigned int i, bool isSelected) {}

    unsigned int getFacesNumber() const;
    unsigned int getFaceValence(unsigned int i) const {return 3;}
    TriIndices getFaceTriIndices(unsigned int i) const ;
    QuatIndices getFaceQuatIndices(unsigned int i) const {return QuatIndices();}
    PolygonIndices getFacePolygonIndices(unsigned int i) const {return PolygonIndices();}
    bool isFaceSelected(unsigned int i) const {return false;}
    void setFaceSelected(unsigned int i, bool isSelected) {}
private:
    int width_;
    int height_;
    Vec3 axis_x_;
    Vec3 axis_y_;
    Vec3 center_;
    std::vector< ColorDepth >   image_;

};
}
#endif // DEPTHIMAGE_H

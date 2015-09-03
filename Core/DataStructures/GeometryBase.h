#ifndef GEOMETRYBASE_H
#define GEOMETRYBASE_H
#include "Core/Math/MathDefines.h"
#include <vector>
namespace GCL{
class GeometryBase
{
public:
    typedef Vec2i EdgeIndices;
    typedef Vec3i TriIndices;
    typedef Vec4i QuatIndices;
    typedef std::vector<int> PolygonIndices;


    virtual unsigned int getPointsNumber() const = 0;
    virtual Vec3 getVertexPosition(unsigned int i) const = 0;
    virtual void setVertexPosition(unsigned int i, const Vec3& pos) = 0;
    virtual Vec3 getVertexNormal(unsigned int i) const = 0;
    virtual void setVertexNormal(unsigned int i, const Vec3& nor) = 0;
    virtual Vec4 getVertexColor(unsigned int i) const = 0;
    virtual void setVertexColor(unsigned int i, const Vec4& color) = 0;
    virtual bool isVertexSelected(unsigned int i) const = 0;
    virtual void setVertexSelected(unsigned int i, bool isSelected) = 0;

    virtual unsigned int getEdgesNumber() const = 0;
    virtual EdgeIndices getEdgeVertexIndices(unsigned int i) const = 0;
    virtual Vec4 getEdgeColor(unsigned int i) const = 0;
    virtual void setEdgeColor(unsigned int i, const Vec4& color) = 0;
    virtual bool isEdgeSelected(unsigned int i) const = 0;
    virtual void setEdgeSelected(unsigned int i, bool isSelected) = 0;

    virtual unsigned int getFacesNumber() const = 0;
    virtual unsigned int getFaceValence(unsigned int i) const = 0;
    virtual TriIndices getFaceTriIndices(unsigned int i) const = 0;
    virtual QuatIndices getFaceQuatIndices(unsigned int i) const = 0;
    virtual PolygonIndices getFacePolygonIndices(unsigned int i) const = 0;
    virtual bool isFaceSelected(unsigned int i) const = 0;
    virtual void setFaceSelected(unsigned int i, bool isSelected) = 0;
};
}

#endif // GEOMETRYBASE_H

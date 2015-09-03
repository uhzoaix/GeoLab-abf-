#ifndef GCL_CTRLPOINTSBASE_H
#define GCL_CTRLPOINTSBASE_H
#include "gcl_global.h"
#include <memory>
#include "GeometryBase.h"
namespace GCL {
/**
 * @brief CtrilPointBase
 * \author Shiwei
 *
 */

class DeformationBase;
class GCL_CORE_SHARED_EXPORT CtrlPointsBase : public GeometryBase
{
public:
    CtrlPointsBase();

public:
    void setDeformation(const std::shared_ptr<DeformationBase>& _deformer);
    DeformationBase* getDeformation() {return deformer_.get();}
    void deformation();

public: /// Geometry Base, for rendering
    virtual unsigned int getPointsNumber() const {return 0;}
    virtual Vec3 getVertexPosition(unsigned int i) const {return Vec3();}
    virtual void setVertexPosition(unsigned int i, const Vec3& pos) {}
    virtual Vec3 getVertexNormal(unsigned int i) const {return Vec3(0,0,1);}
    virtual void setVertexNormal(unsigned int i, const Vec3& nor) {}
    virtual Vec4 getVertexColor(unsigned int i) const {return Vec4();}
    virtual void setVertexColor(unsigned int i, const Vec4& color) {}
    virtual bool isVertexSelected(unsigned int i) const {return false;}
    virtual void setVertexSelected(unsigned int i, bool isSelected) {}

    virtual unsigned int getEdgesNumber() const {return 0;}
    virtual EdgeIndices getEdgeVertexIndices(unsigned int i) const {return EdgeIndices();}
    virtual Vec4 getEdgeColor(unsigned int i) const {return Vec4();}
    virtual void setEdgeColor(unsigned int i, const Vec4& color) {}
    virtual bool isEdgeSelected(unsigned int i) const {return false;}
    virtual void setEdgeSelected(unsigned int i, bool isSelected) {}

    virtual unsigned int getFacesNumber() const {return 0;}
    virtual unsigned int getFaceValence(unsigned int i) const {return 0;}
    virtual TriIndices getFaceTriIndices(unsigned int i) const {return TriIndices();}
    virtual QuatIndices getFaceQuatIndices(unsigned int i) const {return QuatIndices();}
    virtual PolygonIndices getFacePolygonIndices(unsigned int i) const {return PolygonIndices();}
    virtual bool isFaceSelected(unsigned int i) const {return false;}
    virtual void setFaceSelected(unsigned int i, bool isSelected) {}
private:
    std::shared_ptr<DeformationBase> deformer_;
};
}
#endif // CTRLPOINTSBASE_H

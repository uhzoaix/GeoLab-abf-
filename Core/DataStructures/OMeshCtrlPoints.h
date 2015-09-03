#ifndef GCL_OMESHCTRLPOINTS_H
#define GCL_OMESHCTRLPOINTS_H
#include "gcl_global.h"
#include <QList>
#include "CtrlPointsBase.h"
#include "OMesh.h"
namespace GCL {

class GCL_CORE_SHARED_EXPORT OMeshCtrlPoints : public CtrlPointsBase
{
public:
    class PointHandle
    {
    public:
        PointHandle():pos_(0),is_selected_(false)
        {
        }

        PointHandle(const OMesh& mesh, const OMesh::VertexHandle& vh)
        {
            pos_ = mesh.point(vh);
            vh_ = vh;
            is_selected_ = false;
        }
        PointHandle(const Vec3& pos, const OMesh::VertexHandle& vh)
            :pos_(pos), vh_(vh),is_selected_(false)
        {
        }

        const Vec3& Position() const {return pos_;}
        Vec3& Position() {return pos_;}
        const OMesh::VertexHandle& VertexHandle() const {return vh_;}
        OMesh::VertexHandle& VertexHandle() {return vh_;}

        bool is_selected() const {return is_selected_;}
        void set_selected(bool t) {is_selected_ = t;}
    private:
        Vec3 pos_;
        OMesh::VertexHandle vh_;
        bool is_selected_;
    };

    OMeshCtrlPoints(const std::shared_ptr<OMesh>& _mesh);
    OMeshCtrlPoints(OMeshCtrlPoints &other);
    const std::shared_ptr<OMesh>& getMesh() {return mesh_;}
    int getPointHandlersNumber() const {return point_handlers_.size();}
    const PointHandle& getPointHandle(unsigned int i) const {return point_handlers_[i];}

    void addPointHandle(const PointHandle& pointHandle);
    void addPointHandle(const OMesh::VertexHandle& _vh);

    void generateFromSelectedPoints();

public:
    virtual unsigned int getPointsNumber() const {return point_handlers_.size();}
    virtual Vec3 getVertexPosition(unsigned int i) const {return point_handlers_[i].Position();}
    virtual void setVertexPosition(unsigned int i, const Vec3& pos) {point_handlers_[i].Position() = pos;}
    virtual Vec4 getVertexColor(unsigned int i) const {return Vec4(1,1,0,1);}
    virtual bool isVertexSelected(unsigned int i) const {return point_handlers_[i].is_selected();}
    virtual void setVertexSelected(unsigned int i, bool isSelected) {point_handlers_[i].set_selected(isSelected);}

protected:
    std::shared_ptr<OMesh> mesh_;
    QList< PointHandle > point_handlers_;
};
}
#endif // OMESHCTRLPOINTS_H

#ifndef GCL_DECORATION_H
#define GCL_DECORATION_H
#include "gcl_global.h"
#include <QString>

namespace GCL { namespace Decoration {

/**
    This is the basic class for rendering decoration elements, e.g. "Arcball", "2D Rectangle"...

    \author Shiwei Wang
**/


class GCL_CORE_SHARED_EXPORT DecorationBase
{
public:
    explicit DecorationBase():is_with_camera_(true) {}
    virtual ~DecorationBase() {}
    const QString& getName() const {return name_;}
    void setName(const QString& name) {name_ = name;}
    virtual void render() = 0;
    bool is_with_camera() const {return is_with_camera_;}
    void set_with_camera(bool t) {is_with_camera_ = t;}
private:
    QString name_;
    bool is_with_camera_;
};
}}
#endif // GLRENDERELEMENT_H

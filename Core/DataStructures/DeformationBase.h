#ifndef GCL_DEFORMATIONBASE_H
#define GCL_DEFORMATIONBASE_H
#include "gcl_global.h"
#include <QString>
namespace GCL {
class GCL_CORE_SHARED_EXPORT DeformationBase
{
public:
    DeformationBase() {}
    virtual ~DeformationBase() {}
    virtual QString getName() const {return ("DeformationBase");}
    virtual void deform() = 0;
};
}
#endif // DEFORMATIONBASE_H

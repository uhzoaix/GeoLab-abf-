#include "CtrlPointsBase.h"
#include "DeformationBase.h"
namespace GCL {
CtrlPointsBase::CtrlPointsBase()
    : deformer_(NULL)
{
}

void CtrlPointsBase::deformation()
{
    if(deformer_.get() == NULL) return;
    deformer_->deform();
}
}

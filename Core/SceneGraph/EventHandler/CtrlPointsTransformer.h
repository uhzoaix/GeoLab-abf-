#ifndef GCL_CTRLPOINTSTRANSFOMER_H
#define GCL_CTRLPOINTSTRANSFOMER_H

#include "EventHandler.h"
namespace GCL { namespace SceneGraph {
class SceneGraph;
class CtrlPointsTransformer : public EventHandler
{
public:
    CtrlPointsTransformer();
    virtual ~CtrlPointsTransformer();
private:
    SceneGraph* scenegraph_;
};
}}
#endif // CTRLPOINTSTRANSFOMER_H

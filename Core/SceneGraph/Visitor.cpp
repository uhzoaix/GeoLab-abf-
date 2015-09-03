#include "Visitor.h"
#include <QtGlobal>


namespace GCL { namespace SceneGraph {

//Visitor::~Visitor() {}

//default implementations
// Use Q_UNUSED to avoid the warnings.
bool Visitor::enter(SceneGraphNode * _sceneGraphNode){Q_UNUSED(_sceneGraphNode) return true;}
void Visitor::leave(SceneGraphNode *_sceneGraphNode){Q_UNUSED(_sceneGraphNode)}

bool Visitor::enter(GroupNode * _groupNode){Q_UNUSED(_groupNode) return true;}
void Visitor::leave(GroupNode * _groupNode){Q_UNUSED(_groupNode)}

bool Visitor::enter(GeometryNode * _geometryNode){Q_UNUSED(_geometryNode) return true;}
void Visitor::leave(GeometryNode * _geometryNode){Q_UNUSED(_geometryNode)}

bool Visitor::enter(TransformationNode * _transformationNode){Q_UNUSED(_transformationNode) return true;}
void Visitor::leave(TransformationNode * _transformationNode){Q_UNUSED(_transformationNode)}

}}

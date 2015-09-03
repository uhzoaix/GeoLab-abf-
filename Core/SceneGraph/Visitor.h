#ifndef GCL_SCENEGRAPH_VISITOR_H
#define GCL_SCENEGRAPH_VISITOR_H
#include "Core/gcl_global.h"

namespace GCL { namespace SceneGraph {

class GroupNode;
class GeometryNode;
class TransformationNode;
class SceneGraphNode;
/*
* The visitor pattern implementation of a scenegraph tree visitor.
* Note that the visitor operates on pointers directly instead on smart pointers.
* The visitor is assumed to perform an operation on the scenegraph in one pass,
* and not store any pointers to nodes internally that might get deleted in the future.
* All the Enter methods return a boolean. This boolean indicates, whether the node should
* pass the visitor over to his children or not!
*/
class GCL_CORE_SHARED_EXPORT Visitor {

public:

  virtual ~Visitor() {}


  virtual bool enter(SceneGraphNode * _sceneGraphNode);
  virtual void leave(SceneGraphNode * _sceneGraphNode);

  virtual bool enter(GroupNode * _groupNode);
  virtual void leave(GroupNode * _groupNode);

  virtual bool enter(GeometryNode * _geometryNode);
  virtual void leave(GeometryNode * _geometryNode);

  virtual bool enter(TransformationNode * _transformationNode);
  virtual void leave(TransformationNode * _transformationNode);

};

}}


#endif // VISITOR_H

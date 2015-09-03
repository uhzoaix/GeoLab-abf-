#ifndef NODETYPE_H
#define NODETYPE_H

namespace GCL {namespace SceneGraph {
    enum NodeBasicType
    {
       SCENEGRAPH_ROOT_NODE   =       0,
       GROUP_NODE   =                 1,
       TRANSFORMATION_NODE  =        2,
       Geometry_Node = 3
    };
}
}
#endif // NODETYPE_H

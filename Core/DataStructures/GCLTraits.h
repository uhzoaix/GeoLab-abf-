#ifndef GCLTRAITS_H
#define GCLTRAITS_H
#include <OpenMesh/Core/Mesh/Traits.hh>
#include "Core/Math/MathDefines.h"
namespace GCL {
struct GCLTraits : public OpenMesh::DefaultTraits  {

    //types
    typedef GCL::Scalar Scalar;
    typedef GCL::Vec3 Point;
    typedef GCL::Vec3 Normal;
    typedef GCL::Vec4 Color;
    typedef GCL::Vec2 TexCoord2D;

    //enable standard properties
    VertexAttributes   (OpenMesh::Attributes::Status|OpenMesh::Attributes::Normal|OpenMesh::Attributes::Color|OpenMesh::Attributes::TexCoord2D);
    HalfedgeAttributes (OpenMesh::Attributes::Status|OpenMesh::Attributes::PrevHalfedge);
    FaceAttributes     (OpenMesh::Attributes::Status|OpenMesh::Attributes::Normal|OpenMesh::Attributes::Color);
    EdgeAttributes     (OpenMesh::Attributes::Status|OpenMesh::Attributes::Color);

};
}
#endif // GCLTRAITS_H

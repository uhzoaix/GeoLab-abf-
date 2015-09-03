#ifndef GCL_IMAGEPASTING_H
#define GCL_IMAGEPASTING_H
#include <Utilities/OpenMeshAlgorithms/gcl_export.h>
#include <QString>
#include <QList>
#include <Core/Math/MathDefines.h>
namespace GCL {
class OMesh;
namespace Utilities {
/**
 * @brief The MeshPasting class
 *      Input:  1. A depth-image
 *              2. A mesh for pasting
 *              3. the center position, x-axis direction for pasting
 *              4. some other  properties
 *      Output: new mesh after pasting
 *
 */
class GCL_OPENMESHALGORITHMS_SHARED_EXPORT ImagePasting
{
public:
    ImagePasting();
    ~ImagePasting();

    void setMesh(OMesh* _mesh) {mesh_ = _mesh;}
private:

private:
    OMesh* mesh_;


    Vec3 center_;
    Vec3 x_axis_;
    Vec3 y_axis_;
};
}}
#endif // GCL_IMAGEPASTING_H

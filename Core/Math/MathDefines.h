#include "Vec.h"
#include "HomoMatrix.h"
#include "Quaternion.h"
#define ZERO_TOLERANCE 1e-6
namespace GCL {
typedef  double Scalar;
typedef  Vec<3,Scalar> Vec3;
typedef  Vec<4,Scalar> Vec4;
typedef  Vec<2,Scalar> Vec2;
typedef  Vec<2,float> Vec2f;
typedef  Vec<3,float> Vec3f;
typedef  Vec<4,float> Vec4f;
typedef  Vec<2,int> Vec2i;
typedef  Vec<3,int> Vec3i;
typedef  Vec<4,int> Vec4i;
typedef  Quaternion<Scalar> Quat;
typedef  Quaternion<float> Quatf;
typedef  HomoMatrix<4,Scalar> HomoMatrix4;
typedef  HomoMatrix<4,float> HomoMatrix4f;
}

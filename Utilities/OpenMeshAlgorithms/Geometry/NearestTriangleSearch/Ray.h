/****************************************************************************
|*  Ray.h
|*
|*  Declaration of the class Ray consisting of a point and a direction.
|*
|*
|*  Thomas Oskam, Michael Eigensatz, Hao Li - Applied Geometry Group ETH Zurich
|*  oskamt@student.ethz.ch, eigensatz@inf.ethz.ch, hli@inf.ethz.ch
\***********************************************************/


#ifndef _RAY_H
#define _RAY_H
#include <Utilities/OpenMeshAlgorithms/gcl_export.h>
#include <Core/Math/MathDefines.h>


namespace GCL { namespace Utilities {


class GCL_OPENMESHALGORITHMS_SHARED_EXPORT Ray {

public:


  Ray(Vec3 _point, Vec3 _direction);

	Ray(void);

	~Ray(void);

  Vec3 getPointOnRay(double t) const;

  Vec3 point;
  Vec3 direction; //should be normalized

	double min_t;
	double max_t;

	// epsilon t to avoid self intersection due to rounding errors
	// if the ray starts exactly on an element
	static const double epsilon_t;

};

}}

#endif //_RAY_H

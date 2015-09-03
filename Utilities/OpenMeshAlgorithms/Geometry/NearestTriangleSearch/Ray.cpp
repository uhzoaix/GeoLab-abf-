/****************************************************************************
|*  Ray.cpp
|*
|*  Definition of the class Ray consisting of a point and a direction.
|*
|*  Thomas Oskam, Michael Eigensatz, Hao Li - Applied Geometry Group ETH Zurich
|*  oskamt@student.ethz.ch, eigensatz@inf.ethz.ch, hli@inf.ethz.ch
\***********************************************************/


#include "Ray.h"

namespace GCL { namespace Utilities {

Ray::Ray(Vec3 _point, Vec3 _direction)
	:point(_point), direction(_direction.normalize()){
  min_t = 0.;
	max_t = 3.4e38;
}

Ray::Ray(void) {
  point = Vec3(0.0, 0.0, 0.0);
  direction = Vec3(0.0, 0.0, 1.0);
	min_t = 0;
	max_t = 3.4e38;
}

Ray::~Ray(void){}

Vec3 Ray::getPointOnRay(double t) const {
	return point + (direction*t);
}

// epsilon t to avoid self intersection due to rounding errors
// if the ray starts exactly on an element
const double Ray::epsilon_t = 0.001;

}}

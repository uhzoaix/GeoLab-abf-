#ifndef GCL_AABB_H
#define GCL_AABB_H
#include "gcl_global.h"
#include "Core/Math/MathDefines.h"
namespace GCL {
/**
 * @brief The AABB class
 * Axis Aligned Bounding Box;
 * \author Shiwei
 */
class GCL_CORE_SHARED_EXPORT AABB
{
public:

  ////(de)construction
  explicit AABB();
  virtual ~AABB();

  ////data access
  inline void set(const Vec3& _min, const Vec3& _max) { min_=_min; max_=_max; }
  inline void setMin(const Vec3& _min) { min_=_min; }
  inline void setMax(const Vec3& _max) { max_=_max; }
  inline const Vec3& getMin() const { return min_; }
  inline const Vec3& getMax() const { return max_; }


  ////convenience routines
  Vec3 getCenter() const;
  Scalar getRadius() const;

  ////getter Setter of empty flag
  // the bounding box is assumed to be empty, as soon as the min/max values go into an invalid state
  bool isEmpty() const;
  void setEmpty();

  ////growing functions
  void extend(const AABB& _aabb);
  void extend(const Vec3& _point);
  inline void extend(const Scalar _vectorX, const Scalar _vectorY, const Scalar _vectorZ)
  {
    extend(Vec3(_vectorX,_vectorY,_vectorZ));
  }


private:
  Vec3 min_;//vector containing the minima of the box at all axes
  Vec3 max_;//vector containing the maxima of the box at all axes

};
}
#endif // AABB_H

#include "AABB.h"
namespace GCL {
AABB::AABB()
{
    setEmpty();
}

AABB::~AABB()
{

}

Vec3 AABB::getCenter() const
{
    return (min_ + max_) / (Scalar) 2;
}

Scalar AABB::getRadius() const
{
    return (max_ - min_).length() / (Scalar)2;
}

bool AABB::isEmpty() const
{
    return (max_[0]<min_[0] || max_[1]<min_[1] || max_[2]<min_[2]);
}

void AABB::setEmpty()
{
    min_ = Vec3(0,0,0);
    max_ = Vec3(-1,-1,-1);
}

////growing functions
 void AABB::extend(const AABB& _aabb)
 {
   if(isEmpty())
   {
     set(_aabb.getMin(),_aabb.getMax());
   }
   else
   {
     const Vec3& min = _aabb.getMin();
     const Vec3& max = _aabb.getMax();
     for(int i=0; i<3; ++i)
     {
       if(min[i] < min_[i])
       {
         min_[i] = min[i];
       }
       if(max[i] > max_[i])
       {
         max_[i] = max[i];
       }
     }
   }
 }


 void AABB::extend(const Vec3& _point){
   if(isEmpty())
   {
     set(_point,_point);
   }
   else
   {
     for(int i=0; i<3; ++i)
     {
       if(_point[i] < min_[i])
       {
         min_[i] = _point[i];
       }
       else if(_point[i] > max_[i])
       {
         max_[i] = _point[i];
       }
     }
   }
 }




}

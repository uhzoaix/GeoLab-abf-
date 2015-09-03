#ifndef QUATERNION_H
#define QUATERNION_H
#include <math.h>
#include "HomoMatrix.h"
#include "Core/Utilities/LogSystem.h"

namespace GCL {


template<class T = float>
class Quaternion
{
public:
    Quaternion(){
        mX = mY = mZ = mW = 0;
    }

    Quaternion(const T& x,const T& y, const T& z,const T& w)
    {
        mX = x;  mY = y; mZ = z; mW = w;
    }

    Quaternion(const Vec<4,T>& v)
    {
        mX = v[0];mY = v[1]; mZ = v[2]; mW = v[3];
    }
    Quaternion(const Vec<3,T>& axis, const T& angle)
    {

        Vec<3,T> taxis(axis);
        if(taxis.Normalize() < std::numeric_limits<T>::min())
        {
            mX = mY = mZ = mW = 0;
            return;
        }
        mX = taxis[0] * sin(angle / 2);
        mY = taxis[1] * sin(angle / 2);
        mZ = taxis[2] * sin(angle / 2);
        mW = cos(angle / 2);
    }



    void print() const
    {
        DebugLog<<mX<<" "<<mY<<" "<<mZ<<" "<<mW<<std::endl;
    }
    T length2() const
    {
        return mX * mX + mY * mY + mZ * mZ + mW * mW;
    }
    T length() const
    {
        sqrt(length2());
    }
    Quaternion<T> conjugate() const
    {
        Quaternion<T> q(-mX,-mY,-mZ,mW);
        return q;
    }
    T Normalize()
    {
        T len = length();
        if(len <= (T)std::numeric_limits<T>::min())
        {
            DebugLog<<"hehre"<<std::endl;
            return len;
        }
        mX /= len;
        mY /= len;
        mZ /= len;
        mW /= len;
        return (T)1;
    }
    Vec<3,T> getAxis() const
    {
        return Vec<3,T>(mX,mY,mZ);
    }
    T getAngle() const
    {
        return acos(mW) * 2;
    }
    void convertFromMatrix(const HomoMatrix<4,T>& m)
    {
        T s;
        T tq[4];
        int i,j;

        // Use tq to store the largest trace
        tq[0] = 1 + m.getValue(0,0)+m.getValue(1,1)+m.getValue(2,2);
        tq[1] = 1 + m.getValue(0,0)-m.getValue(1,1)-m.getValue(2,2);
        tq[2] = 1 - m.getValue(0,0)+m.getValue(1,1)-m.getValue(2,2);
        tq[3] = 1 - m.getValue(0,0)-m.getValue(1,1)+m.getValue(2,2);

        // Find the maximum (could also use stacked if's later)
        j = 0;
        for(i=1;i<4;i++) j = (tq[i]>tq[j])? i : j;

        // check the diagonal
        if (j==0)
        {
            /* perform instant calculation */
            mW = tq[0];
            mX = m.getValue(1,2)-m.getValue(2,1);
            mY = m.getValue(2,0)-m.getValue(0,2);
            mZ = m.getValue(0,1)-m.getValue(1,0);
        }
        else if (j==1)
        {
            mW = m.getValue(1,2)-m.getValue(2,1);
            mX = tq[1];
            mY = m.getValue(0,1)+m.getValue(1,0);
            mZ = m.getValue(2,0)+m.getValue(0,2);
        }
        else if (j==2)
        {
            mW = m.getValue(2,0)-m.getValue(0,2);
            mX = m.getValue(0,1)+m.getValue(1,0);
            mY = tq[2];
            mZ = m.getValue(1,2)+m.getValue(2,1);
        }
        else /* if (j==3) */
        {
            mW = m.getValue(0,1)-m.getValue(1,0);
            mX = m.getValue(2,0)+m.getValue(0,2);
            mY = m.getValue(1,2)+m.getValue(2,1);
            mZ = tq[3];
        }
        s = sqrt(0.25/tq[j]);
        mW *= s;
        mX *= -s;
        mY *= -s;
        mZ *= -s;
    }

    HomoMatrix<4,T> convertToMatrix() const
    {
        HomoMatrix<4,T> mat;
        T s, xs, ys, zs, wx, wy, wz, xx, xy, xz, yy, yz, zz;
        T sum = ((mX * mX) + (mY * mY) +
                 (mZ * mZ) + (mW * mW));
        if(fabs(sum) < std::numeric_limits<T>::min())
        {
            return mat;
        }
        s = 2.0 / sum;

        xs = mX * s;
        ys = mY * s;
        zs = mZ * s;

        wx = mW * xs;
        wy = mW * ys;
        wz = mW * zs;

        xx = mX * xs;
        xy = mX * ys;
        xz = mX * zs;

        yy = mY * ys;
        yz = mY * zs;

        zz = mZ * zs;



        /*!
         Fill in matrix

       */
        mat.setValue(0,0,1.0 - (yy + zz));
        mat.setValue(0,1, xy - wz);
        mat.setValue(0,2 , xz + wy);

        mat.setValue(1,0, xy + wz);
        mat.setValue(1,1, 1.0 - (xx + zz));
        mat.setValue(1,2,yz - wx);

        mat.setValue(2,0,  xz - wy);
        mat.setValue(2,1,yz + wx);
        mat.setValue(2,2,1.0 - (xx + yy));

        for(int i=0; i < 3; i++)
        {
            mat.setValue(i,3,0);
            mat.setValue(3,i,0);
        }
        mat.setValue(3,3,1);
        return mat;

    }
    const T& x() const {return mX;}
    const T& y() const {return mY;}
    const T& z() const {return mZ;}
    const T& w() const {return mW;}
    T& x() {return mX;}
    T& y() {return mY;}
    T& z() {return mZ;}
    T& w() {return mW;}

    static  Quaternion<T> quatFromVectorTransform(const Vec<3,T>& tv0, const Vec<3,T>& tv1)
    {
        Vec<3,T> v0(tv0);
        Vec<3,T> v1(tv1);
        v0.normalize();
        v1.normalize();
        Vec<3,T> axis = v0 ^ v1;
        if(axis.Normalize() < std::numeric_limits<T>::min())
        {
            axis = v0 ^ Vec<3,T>(1,0,0);
            if(axis.Normalize() < std::numeric_limits<T>::min())
            {
                axis = v0 ^ Vec<3,T>(0,1,0);
            }
        }
        T angle = acos(v0 * v1);
        Quaternion<T> ans(axis,angle);
        return ans;
    }
private:
    T mX,mY,mZ,mW;
};

template <class T>
static inline const Quaternion<T> operator + (const Quaternion<T>  &v1, const Quaternion<T>  &v2)
{
    Quaternion<T>  result;
    result.x() = v1.x() + v2.x();
    result.y() = v1.y() + v2.y();
    result.z() = v1.z() + v2.z();
    result.w() = v1.w() + v2.w();
    return result;
}

template <class T>
static inline const Quaternion<T> operator - (const Quaternion<T>  &v1, const Quaternion<T>  &v2)
{
    Quaternion<T>  result;
    result.x() = v1.x() - v2.x();
    result.y() = v1.y() - v2.y();
    result.z() = v1.z() - v2.z();
    result.w() = v1.w() - v2.w();
    return result;
}

template <class T>
static inline const Quaternion<T> operator * (const Quaternion<T>  &left, const Quaternion<T>  &right)
{
    Quaternion<T> ans;
    T d1, d2, d3, d4;

    d1 =  left.w() * right.w();
    d2 = -left.x() * right.x();
    d3 = -left.y() * right.y();
    d4 = -left.z() * right.z();
    ans.w() = d1+ d2+ d3+ d4;

    d1 =  left.w() * right.x();
    d2 =  right.w() * left.x();
    d3 =  left.y() * right.z();
    d4 = -left.z() * right.y();
    ans.x() =  d1+ d2+ d3+ d4;

    d1 =  left.w() * right.y();
    d2 =  right.w() * left.y();
    d3 =  left.z() * right.x();
    d4 = -left.x() * right.z();
    ans.y() =  d1+ d2+ d3+ d4;

    d1 =  left.w() * right.z();
    d2 =  right.w() * left.z();
    d3 =  left.x() * right.y();
    d4 = -left.y() * right.x();
    ans.z() =  d1+ d2+ d3+ d4;
    return ans;
}

template <class T>
static inline const Vec<3,T> operator * (const Quaternion<T>  &quat0, const Vec<3,T>  &v)
{
    Quaternion<T> quat1(v[0],v[1],v[2],0);
    Quaternion<T> ans = quat0 * quat1;
    ans = ans * quat0.conjugate();
    Vec<3,T> ansV(ans.x(),ans.y(),ans.z());
    return ansV;
}

}
#endif // QUATERNION_H

#ifndef HOMOMATRIX_H
#define HOMOMATRIX_H
#include "Vec.h"
#include "assert.h"
#include "Core/Utilities/LogSystem.h"

namespace GCL {
template<size_t D, class T = float>
class HomoMatrix
{
public:
    HomoMatrix()
    {
        Unit();
    }

    HomoMatrix(const HomoMatrix<D,T>& mat)
    {
        for(size_t i=0; i < D * D; i++)
        {
            v[i] = mat.at(i);
        }
    }
    HomoMatrix(const HomoMatrix<D-1,T>& mat)
    {
        Unit();
        for(size_t i=0; i < D-1; i++)
        {
            for(size_t j=0; j < D-1; j++)
            {
                setValue(i,j,mat.getValue(i,j));
            }
        }
    }
    HomoMatrix(const Vec<D,T>& quat)
    {
        VEC_STATIC_CHECK(D==4);
        T x2 = quat[0] * quat[0];
        T y2 = quat[1] * quat[1];
        T z2 = quat[2] * quat[2];
        T xy = quat[0] * quat[1];
        T xz = quat[0] * quat[2];
        T yz = quat[1] * quat[2];
        T wx = quat[3] * quat[0];
        T wy = quat[3] * quat[1];
        T wz = quat[3] * quat[2];
        // This calculation would be a lot more complicated for non-unit length quaternions
        // Note: The constructor of Matrix4 expects the Matrix in column-major format like expected by
        // OpenGL
        v[0] = 1.0 - 2.0 * (y2 + z2);
        v[1] = 2.0 * (xy - wz);
        v[2] = 2.0 * (xz + wy);
        v[3] = 0.0;
        v[4] = 2.0 * (xy + wz);
        v[5] = 1.0 - 2.0 * (x2 + z2);
        v[6] = 2.0 * (yz - wx);
        v[7] = 0.0;
        v[8] = 2.0 * (xz - wy);
        v[9] = 2.0 * (yz + wx);
        v[10] = 1.0 - 2.0 * (x2 + y2);
        v[11] = 0.0;
        v[12] = 0.0;
        v[13] = 0.0;
        v[14] = 0.0;
        v[15] = 1.0;
    }

    HomoMatrix(const T* pV)
    {
        for(size_t i=0; i < D * D; i++)
        {
            v[i] = pV[i];
        }
    }
    const T* getPtr() const
    {
        return v;
    }
    const T* data() const
    {
        return v;
    }
    void print() const
    {
        DebugLog<<"Matrix:"<<std::endl;
        for(size_t i=0; i < D; i++)
        {
            for(size_t j=0; j < D; j++)
            {
                if(fabs(getValue(i,j)) < std::numeric_limits<T>::min())
                {
                    DebugLog<<"0 ";
                    continue;
                }
                DebugLog<<getValue(i,j)<<" ";
            }
            DebugLog<<std::endl;
        }
    }

    void Unit()
    {
        for(size_t i=0; i < D; i++)
        {
            for(size_t j=0; j < D; j++)
            {
                v[i * D + j] = (T)(i==j);
            }
        }
    }
    T& operator [] (size_t i)
    {
        if(i>=D * D) throw std::out_of_range("Matrix::[]");
        return v[i];
    }
    T& operator [] (int i)
    {
        if(i>=D * D) throw std::out_of_range("Matrix::[]");
        return v[i];
    }
    const T& operator [] (size_t i) const
    {
        if(i>=D * D) throw std::out_of_range("Matrix::[]");
        return v[i];
    }
    const T& operator [] (int i) const
    {
        if(i>=D * D) throw std::out_of_range("Matrix::[]");
        return v[i];
    }
    T at(int i) const
    {
        assert(i < D*D && i>=0);
        return v[i];
    }
    T at(size_t i) const
    {
        assert(i < D*D);
        return v[i];
    }

    // Note: The constructor of Matrix4 expects the Matrix in column-major format like expected by
    // OpenGL
    T getValue(int row, int col) const
    {
        assert(row < D && col < D && row >= 0 && col >= 0);
        return v[D * col + row];
    }
    T getValue(size_t row, size_t col) const
    {
        assert(row < D && col < D);
        return v[D * col + row];
    }
    void setValue(int row, int col, const T& val)
    {
        assert(row < D && col < D && row >= 0 && col >= 0);
        v[D * col + row] = val;
    }
    void setValue(size_t row, size_t col, const T& val)
    {
        assert(row < D && col < D);
        v[D * col + row] = val;
    }

    HomoMatrix<D,T> operator * (const HomoMatrix<D,T>& mat) const
    {
        HomoMatrix<D,T> result;
        for(size_t i=0; i < D; i++)
        {
            for(size_t j=0; j < D; j++)
            {
                T ans = (T)0;
                for(size_t k=0; k < D; k++)
                {
                    ans += getValue(i,k) * mat.getValue(k,j);
                }
                result.setValue(i,j,ans);
            }
        }
        return result;
    }
    HomoMatrix<D,T> operator + (const HomoMatrix<D,T>& mat) const
    {
        HomoMatrix<D,T> result;
        for(size_t i=0; i < D; i++)
        {
            for(size_t j=0; j < D; j++)
            {
                result.setValue(i,j,getValue(i,j) + mat.getValue(i,j));
            }
        }
        return result;
    }
    HomoMatrix<D,T> operator - (const HomoMatrix<D,T>& mat) const
    {
        HomoMatrix<D,T> result;
        for(size_t i=0; i < D; i++)
        {
            for(size_t j=0; j < D; j++)
            {
                result.setValue(i,j,getValue(i,j) - mat.getValue(i,j));
            }
        }
        return result;
    }

    Vec<D,T> operator * (const Vec<D,T>& vec) const
    {
        Vec<D,T> result;
        for(size_t i=0; i < D; i++)
        {
            for(size_t j=0; j < D; j++)
            {
                result[i] += getValue(i,j) * vec[j];
            }
        }
        return result;
    }

    Vec<D-1,T> operator * (const Vec<D-1,T>& vec) const
    {
        VEC_STATIC_CHECK(D>=2);
        Vec<D-1,T> result;
        for(size_t i=0; i < D-1; i++)
        {
            for(size_t j=0; j < D-1; j++)
            {
                result[i] += getValue(i,j) * vec[j];
            }
        }
        return result;
    }
    Vec<D,T> transformVector(const Vec<D,T>& vec) const
    {
        Vec<D,T> result;
        for(size_t i=0; i < D; i++)
        {
            for(size_t j=0; j < D; j++)
            {
                result[i] += getValue(i,j) * vec[j];
            }
        }
        return result;
    }
    Vec<D-1,T> transformVector(const Vec<D-1,T>& vec) const
    {
        VEC_STATIC_CHECK(D>=2);
        Vec<D-1,T> result;
        for(size_t i=0; i < D-1; i++)
        {
            for(size_t j=0; j < D-1; j++)
            {
                result[i] += getValue(i,j) * vec[j];
            }
            result[i] = result[i] + getValue(i,D-1);
        }
        return result;
    }
    Vec<D-1,T> RotateVector(const Vec<D-1,T>& vec) const
    {
        VEC_STATIC_CHECK(D>=2);
        Vec<D-1,T> result;
        for(size_t i=0; i < D-1; i++)
        {
            for(size_t j=0; j < D-1; j++)
            {
                result[i] += getValue(i,j) * vec[j];
            }
        }
        return result;
    }
    HomoMatrix<D,T> operator * (const T& val) const
    {
        HomoMatrix<D,T> result;
        for(size_t i=0; i < D; i++)
        {
            for(size_t j=0; j < D; j++)
            {
                result.setValue(i,j,getValue(i,j) * val);
            }
        }
        return result;
    }


    HomoMatrix<D,T> operator += (const HomoMatrix<D,T>& mat)
    {
        for(size_t i=0; i < D; i++)
        {
            for(size_t j=0; j < D; j++)
            {
                setValue(i,j,getValue(i,j) + mat.getValue(i,j));
            }
        }
        return *this;
    }
    HomoMatrix<D,T> operator -= (const HomoMatrix<D,T>& mat)
    {
        for(size_t i=0; i < D; i++)
        {
            for(size_t j=0; j < D; j++)
            {
                setValue(i,j,getValue(i,j) - mat.getValue(i,j));
            }
        }
        return *this;
    }
    HomoMatrix<D,T> operator *= (const HomoMatrix<D,T>& mat)
    {
        HomoMatrix<D,T> matCopy = *this;
        HomoMatrix<D,T> result;
        for(size_t i=0; i < D; i++)
        {
            for(size_t j=0; j < D; j++)
            {
                T ans = (T)0;
                for(size_t k=0; k < D; k++)
                {
                    ans += matCopy.getValue(i,k) * mat.getValue(k,j);
                }
                setValue(i,j,ans);
            }
        }
        return *this;
    }


private:
    T v[D*D];
};
}




#endif // HOMOMATRIX_H

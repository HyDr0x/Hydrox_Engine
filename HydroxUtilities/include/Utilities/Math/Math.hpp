#ifndef MATH_HPP_
#define MATH_HPP_

#include <math.h>

#include "Matrix.hpp"
#include "Quaternion.hpp"

namespace he
{
	namespace util
	{
    namespace math//make the functions inline or create a declaration cpp (no double defined functions allowed in c++)
    {
      static const float PI = 3.141592654f;
      static const float PI_HALF = PI / 2.0f;
      static const float PI_QUARTER = PI_HALF / 2.0f;

      template<typename Type> inline Type clamp(Type a, Type intervalStart, Type intervalEnd)
      {
        Type tmpA = a < intervalStart ? intervalStart : a;
        Type tmpB = a > intervalEnd ? intervalEnd : a;
        return a < intervalStart ? intervalStart : a > intervalEnd ? intervalEnd : a;
      }

      inline float round(float val)
      {
        return (float)static_cast<int>(val + 0.5f);
      }

      inline float degToRad( float angle )
      {
        return angle / 180.0f * PI;
      }

      inline float radToDeg( float angle )
      {
        return angle / PI * 180.0f;
      }

	    template<typename Type> inline Vector<Type, 4> abs(Vector<Type, 4>& v1)
      {
        return Vector<Type,4>(abs(v1[0]), abs(v1[1]), abs(v1[2]), abs(v1[3]));
      }

      template<typename Type> inline Vector<Type, 3> abs(Vector<Type, 3>& v1)
      {
        return Vector<Type,3>(abs(v1[0]), abs(v1[1]), abs(v1[2]));
      }

      template<typename Type> inline Vector<Type, 2> abs(Vector<Type, 2>& v1)
      {
        return Vector<Type,2>(abs(v1[0]),abs(v1[1]));
      }

      template<typename Type> inline Vector<Type, 3> cross(Vector<Type, 3>& v1, Vector<Type, 3>& v2)
      {
        return Vector<Type,3>(v1[1]*v2[2]-v2[1]*v1[2], v1[2]*v2[0]-v1[0]*v2[2], v1[0]*v2[1]-v1[1]*v2[0]);
      }

      template<typename CastType, typename Type> inline Vector<CastType, 4> vector_cast(Vector<Type, 4>& v)
      {
        return Vector<CastType, 4>(static_cast<CastType>(v[0]), static_cast<CastType>(v[1]), static_cast<CastType>(v[2]), static_cast<CastType>(v[3]));
      }

      template<typename CastType, typename Type> inline Vector<CastType, 3> vector_cast(Vector<Type, 3>& v)
      {
        return Vector<CastType, 3>(static_cast<CastType>(v[0]), static_cast<CastType>(v[1]), static_cast<CastType>(v[2]));
      }

      template<typename CastType, typename Type> inline Vector<CastType, 2> vector_cast(Vector<Type, 2>& v)
      {
        return Vector<CastType, 2>(static_cast<CastType>(v[0]), static_cast<CastType>(v[1]));
      }

      inline Matrix<float, 4> createPerspective(float left, float right, float bottom, float top, float cnear, float cfar)
      {
        return Matrix<float, 4>((2.0f * cnear) / (right - left), 0.0f,                            (right + left) / (right - left), 0.0f,
                                 0.0f,                           (2.0f * cnear) / (top - bottom), (top + bottom) / (top - bottom), 0.0f,
                                 0.0f,                           0.0f,                            (cfar + cnear) / (cfar - cnear), (2.0f * cfar * cnear) / (cfar - cnear),
                                 0.0f,                           0.0f,                            -1.0f,                           0.0f);
      }

      inline Matrix<float, 4> createPerspective(float fov, float aspectRatio, float cnear, float cfar)
      {
        float f = 1.0f / tanf(fov / 2.0f);

        return Matrix<float,4>(f / aspectRatio,             0.0f,                       0.0f,                               0.0f,
                               0.0f,                        f,                          0.0f,                               0.0f,
                               0.0f,                        0.0f,                       (cfar + cnear) / (cfar - cnear),    (2.0f * cfar * cnear) / (cfar - cnear),
                               0.0f,                        0.0f,                       -1.0f,                              0.0f);
      }

      inline Matrix<float, 4> createOrthographic(float left, float right, float bottom, float top, float cnear, float cfar)
      {
        return Matrix<float, 4>(2.0f / (right - left), 0.0f,                   0.0f,                   -(left + right) / (right - left),
                                0.0f,                  2.0f / (top - bottom),  0.0f,                   -(bottom + top) / (top - bottom),
                                0.0f,                  0.0f,                  -2.0f / (cfar - cnear),  -(cnear + cfar) / (cfar - cnear),
                                0.0f,                  0.0f,                   0.0f,                   1.0f);
      }

      inline Matrix<float, 4> createLookAt(Vector<float, 3> camPos, Vector<float, 3> forwardVector, Vector<float, 3> upVektor)
      {
        Vector<float, 3> z = forwardVector;

        z.normalize();
        upVektor.normalize();
        Vector<float, 3> x = cross(upVektor, z);
        x.normalize();
        Vector<float, 3> y = cross(z, x);

        Matrix<float, 4> rotMat(x[0], x[1], x[2], 0.0f,
                                y[0], y[1], y[2], 0.0f,
                                z[0], z[1], z[2], 0.0f,
                                0.0f, 0.0f, 0.0f, 1.0f);

        rotMat[3] = rotMat[0] * -camPos[0] + rotMat[1] * -camPos[1] + rotMat[2] * -camPos[2] + rotMat[3];

        return rotMat; 
      }

      template<typename Type> Matrix<Type,4> rotAxis(Matrix<Type, 4> m, float angle, Vector<Type, 3>& v)
      {
        angle = degToRad(angle);
        const float sinAngle = sinf(angle);
        const float cosAngle = cosf(angle);
        v.normalize();
        Vector<Type, 3> vTmp = v * (1.0f - cosAngle);

        Matrix<Type, 4> rotMat( cosAngle + vTmp[0] * v[0]                  , 0.0f + vTmp[0] * v[1] + sinAngle * v[2], 0.0f + vTmp[0] * v[2] - sinAngle * v[1], 0.0f,
                                0.0f     + vTmp[1] * v[0] - sinAngle * v[2], cosAngle + vTmp[1] * v[1]              , 0.0f + vTmp[1] * v[2] + sinAngle * v[0], 0.0f,
                                0.0f     + vTmp[2] * v[0] + sinAngle * v[1], 0.0f + vTmp[2] * v[1] - sinAngle * v[0], cosAngle + vTmp[2] * v[2]              , 0.0f,
                                0.0f                                       , 0.0f                                   , 0.0f                                   , 1.0f);

        Matrix<Type, 4> result;
        result[0] = rotMat[0] * m[0][0] + rotMat[1] * m[0][1] + rotMat[2] * m[0][2];
        result[1] = rotMat[0] * m[1][0] + rotMat[1] * m[1][1] + rotMat[2] * m[1][2];
        result[2] = rotMat[0] * m[2][0] + rotMat[1] * m[2][1] + rotMat[2] * m[2][2];
        result[3] = m[3];

        return result;
      }

      template<typename Type> void decomposeMatrix(Matrix<Type, 4> matrix, Vector<Type, 3>& angle, Vector<Type, 3>& position, Vector<Type, 3>& scale)
      {
        scale[0] = sqrt(matrix[0][0] * matrix[0][0] + matrix[0][1] * matrix[0][1] + matrix[0][2] * matrix[0][2]);
        scale[1] = sqrt(matrix[1][0] * matrix[1][0] + matrix[1][1] * matrix[1][1] + matrix[1][2] * matrix[1][2]);
        scale[2] = sqrt(matrix[2][0] * matrix[2][0] + matrix[2][1] * matrix[2][1] + matrix[2][2] * matrix[2][2]);

        matrix[0][0] /= scale[0];
        matrix[0][1] /= scale[0];
        matrix[0][2] /= scale[0];

        matrix[1][0] /= scale[1];
        matrix[1][1] /= scale[1];
        matrix[1][2] /= scale[1];

        matrix[2][0] /= scale[2];
        matrix[2][1] /= scale[2];
        matrix[2][2] /= scale[2];

        angle[0] = asin(matrix[2][1]);
        angle[1] = asin(matrix[0][2]);
        angle[2] = asin(matrix[1][0]);

        Matrix<float, 4> rtxMatrix, rtyMatrix, rtzMatrix;
        rtxMatrix = rtyMatrix = rtzMatrix = Matrix<float, 4>::identity();

        rtxMatrix[1][1] = rtxMatrix[2][2] = cos(angle[0]);
        rtxMatrix[2][1] = sin(angle[0]);
        rtxMatrix[1][2] = -rtxMatrix[2][1];
        rtxMatrix = rtxMatrix.invert();

	      rtyMatrix[0][0] = rtyMatrix[2][2] = cos(angle[1]);
        rtyMatrix[0][2] = sin(angle[1]);
        rtyMatrix[2][0] = -rtyMatrix[0][2];
        rtyMatrix = rtyMatrix.invert();

	      rtzMatrix[0][0] = rtzMatrix[1][1] = cos(angle[2]);
        rtzMatrix[1][0] = sin(angle[2]);
        rtzMatrix[0][1] = -rtzMatrix[1][0];
        rtzMatrix = rtzMatrix.invert();

        matrix *= rtxMatrix * rtyMatrix * rtzMatrix;

        position[0] = matrix[3][0] / scale[0];
        position[1] = matrix[3][1] / scale[1];
        position[2] = matrix[3][2] / scale[2];
      }

      template<typename Type> inline Quaternion<Type> createRotXQuaternion(Type angle)
      {
        angle *= 0.5f;
        return Quaternion<Type>(cosf(angle), sinf(angle), 0.0f, 0.0f);
      }

      template<typename Type> inline Quaternion<Type> createRotYQuaternion(Type angle)
      {
        angle *= 0.5f;
        return Quaternion<Type>(cosf(angle), 0.0f, sinf(angle), 0.0f);
      }

      template<typename Type> inline Quaternion<Type> createRotZQuaternion(Type angle)
      {
        angle *= 0.5f;
        return Quaternion<Type>(cosf(angle), 0.0f, 0.0f, sinf(angle));
      }

      template<typename Type> inline Quaternion<Type> createRotAxisQuaternion(Type angle, Vector<Type, 3> axis)
      {
        angle *= 0.5f;
        axis *= sinf(angle);

        return Quaternion<Type>(cosf(angle), axis[0], axis[1], axis[2]);
      }

      inline Matrix<float, 4> createTransformationMatrix(Vector<float, 3> translation, float scale, Quaternion<float> rotation)
      {
        Matrix<float, 4> trfMatrix;

        trfMatrix = rotation.toMatrix();

        trfMatrix *= scale;
        trfMatrix[3][3] = 1.0f;

        trfMatrix[3][0] = translation[0];
        trfMatrix[3][1] = translation[1];
        trfMatrix[3][2] = translation[2];

        return trfMatrix;
      }

      #if defined(SSE4)
      template<typename Type> inline Vector<Type, 4> operator * ( Vector<Type, 4>& v, Matrix<Type, 4>& m )//SSE4.1
      {
        __m128 a,b,c;
        Vector<Type, 4> vTmp, vErg;
        b=_mm_loadu_ps(&v[0]);

        a=_mm_loadu_ps(&(m[0][0]));
        c=_mm_dp_ps(a,b,0xF1);
        _mm_storeu_ps(&vErg[0],c);

        a=_mm_loadu_ps(&(m[1][0]));
        c=_mm_dp_ps(a,b,0xF2);
        _mm_storeu_ps(&vTmp[0],c);
        vErg[1] = vTmp[1];

        a=_mm_loadu_ps(&(m[2][0]));
        c=_mm_dp_ps(a,b,0xF4);
        _mm_storeu_ps(&vTmp[0],c);
        vErg[2] = vTmp[2];

        a=_mm_loadu_ps(&(m[3][0]));
        c=_mm_dp_ps(a,b,0xF8);
        _mm_storeu_ps(&vTmp[0],c);
        vErg[3] = vTmp[3];

        return vErg;
      }

      template<typename Type> inline Vector<Type, 3> operator * ( Vector<Type, 3>& v, Matrix<Type, 3>& m )//SSE4.1
      {
        __m128 a,b,c;
        Vector<Type, 3> vTmp, vErg;
        b=_mm_loadu_ps(&v[0]);

        a=_mm_loadu_ps(&(m[0][0]));
        c=_mm_dp_ps(a,b,0xF1);
        _mm_storeu_ps(&vErg[0],c);

        a=_mm_loadu_ps(&(m[1][0]));
        c=_mm_dp_ps(a,b,0xF2);
        _mm_storeu_ps(&vTmp[0],c);
        vErg[1] = vTmp[1];

        a=_mm_loadu_ps(&(m[2][0]));
        c=_mm_dp_ps(a,b,0xF4);
        _mm_storeu_ps(&vTmp[0],c);
        vErg[2] = vTmp[2];

        return vErg;
      }

      template<typename Type> inline Vector<Type, 2> operator * ( Vector<Type, 2>& v, Matrix<Type, 2>& m )//SSE4.1
      {
        __m128 a,b,c;
        Vector<Type, 2> vTmp, vErg;
        b=_mm_loadu_ps(&v[0]);

        a=_mm_loadu_ps(&(m[0][0]));
        c=_mm_dp_ps(a,b,0xF1);
        _mm_storeu_ps(&vErg[0],c);

        a=_mm_loadu_ps(&(m[1][0]));
        c=_mm_dp_ps(a,b,0xF2);
        _mm_storeu_ps(&vTmp[0],c);
        vErg[1] = vTmp[1];

        return vErg;
      }
    #endif
    }
	}
}

#endif

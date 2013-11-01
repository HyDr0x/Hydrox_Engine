#ifndef MATH_H_
#define MATH_H_

#include <math.h>

#include "Matrix.hpp"

namespace math
{
  static const float PI = 3.141592654f;
  static const float PI_HALF = PI / 2.0f;
  static const float PI_QUARTER = PI_HALF / 2.0f;

  __forceinline float round(float val)
  {
    return (float)static_cast<int>(val + 0.5f);
  }

  __forceinline float degToRad( float angle )
  {
    return angle / 180.0f * PI;
  }

  __forceinline float radToDeg( float angle )
  {
    return angle / PI * 180.0f;
  }

	template<typename Type> Vec<Type,4> abs(Vec<Type,4>& v1)
  {
    return Vec<Type,4>(abs(v1[0]),abs(v1[1]),abs(v1[2]),abs(v1[3]));
  }

  template<typename Type> Vec<Type,3> abs(Vec<Type,3>& v1)
  {
    return Vec<Type,3>(abs(v1[0]),abs(v1[1]),abs(v1[2]));
  }

  template<typename Type> Vec<Type,2> abs(Vec<Type,2>& v1)
  {
    return Vec<Type,2>(abs(v1[0]),abs(v1[1]));
  }

  template<typename Type> Vec<Type,1> abs(Vec<Type,1>& v1)
  {
    return Vec<Type,1>(abs(v1[0]));
  }

  template<typename Type> Vec<Type,3> cross(Vec<Type,3>& v1, Vec<Type,3>& v2)
  {
    return Vec<Type,3>(v1[1]*v2[2]-v2[1]*v1[2], v1[2]*v2[0]-v1[0]*v2[2], v1[0]*v2[1]-v1[1]*v2[0]);
  }

  template<typename CastType, typename Type> Vec<CastType, 4> vector_cast(Vec<Type, 4>& v)
  {
    return Vec<CastType, 4>(static_cast<CastType>(v[0]), static_cast<CastType>(v[1]), static_cast<CastType>(v[2]), static_cast<CastType>(v[3]));
  }

  template<typename CastType, typename Type> Vec<CastType, 3> vector_cast(Vec<Type, 3>& v)
  {
    return Vec<CastType, 3>(static_cast<CastType>(v[0]), static_cast<CastType>(v[1]), static_cast<CastType>(v[2]));
  }

  template<typename CastType, typename Type> Vec<CastType, 2> vector_cast(Vec<Type, 2>& v)
  {
    return Vec<CastType, 2>(static_cast<CastType>(v[0]), static_cast<CastType>(v[1]));
  }

  template<typename CastType, typename Type> Vec<CastType, 1> vector_cast(Vec<Type, 1>& v)
  {
    return Vec<CastType, 1>(static_cast<CastType>(v[0]));
  }

  inline Mat<float,4> createPerspective(float left, float right, float bottom, float top, float cnear, float cfar)
  {
    return Mat<float,4>((2.0f * cnear) / (right - left), 0.0f,                           (right + left) / (right - left), 0.0f,
                        0.0f,                           (2.0f * cnear) / (top - bottom), (top + bottom) / (top - bottom), 0.0f,
                        0.0f,                           0.0f,                           (cfar + cnear) / (cfar - cnear),     (2.0f * cfar * cnear) / (cfar - cnear),
                        0.0f,                           0.0f,                          -1.0f,                            0.0f);
  }

  inline Mat<float,4> createPerspective(float fov, float aspectRatio, float cnear, float cfar)
  {
    float f = 1.0f / tanf(fov / 2.0f);

    return Mat<float,4>(f / aspectRatio,                0.0f,                          0.0f,                               0.0f,
                        0.0f,                           f,                             0.0f,                               0.0f,
                        0.0f,                           0.0f,                          (cfar + cnear) / (cfar - cnear),       (2.0f * cfar * cnear) / (cfar - cnear),
                        0.0f,                           0.0f,                          -1.0f,                              0.0f);
  }

  //inline Mat<float,4> createPerspective(float fov, float aspectRatio, float near, float far)
  //{
  //  float f = 1.0f / tanf(fov / 2.0f);

  //  return Mat<float,4>(f / aspectRatio,                0.0f,                          0.0f,                               0.0f,
  //                      0.0f,                           f,                             0.0f,                               0.0f,
  //                      0.0f,                           0.0f,                          (far + near) / (near - far),       -1.0f,
  //                      0.0f,                           0.0f,                          (2.0f * far * near) / (near - far), 0.0f);
  //}

  inline Mat<float,4> createOrthographic(float left, float right, float bottom, float top, float cnear, float cfar)
  {
    return Mat<float,4>(2.0f / (right - left),0.0f,                   0.0f,                -(left + right) / (right - left),
                        0.0f,                 2.0f / (top - bottom),  0.0f,                -(bottom + top) / (top - bottom),
                        0.0f,                 0.0f,                  -2.0f / (cfar - cnear), -(cnear + cfar)   / (cfar - cnear),
                        0.0f,                 0.0f,                   0.0f,                 1.0f);
  }

  inline Mat<float,4> createLookAt(Vec<float,3> camPos, Vec<float,3> aimPos, Vec<float,3> upVektor)
  {
    Vec<float,3> z = aimPos - camPos;
    z.normalize();
    upVektor.normalize();
    Vec<float,3> x = cross(z, upVektor);
    x.normalize();
    Vec<float,3> y = cross(x, z);

    Mat<float,4> rotMat(x[0], x[1], x[2], 0,
                        y[0], y[1], y[2], 0,
                        -z[0],-z[1],-z[2], 0,
                        0.0f, 0.0f, 0.0f, 1.0f);


    rotMat[3] = rotMat[0] * -camPos[0] + rotMat[1] * -camPos[1] + rotMat[2] * -camPos[2] + rotMat[3];

    return rotMat;
  }

  template<typename Type> Mat<Type,4> rotAxis(Mat<Type,4> m, float angle, Vec<Type,3>& v)
  {
    angle = degToRad(angle);
    const float sinAngle = sinf(angle);
    const float cosAngle = cosf(angle);
    v.normalize();
    Vec<Type,3> vTmp = v * (1.0f - cosAngle);

    Mat<Type,4> rotMat(cosAngle + vTmp[0] * v[0]                  , 0.0f + vTmp[0] * v[1] + sinAngle * v[2], 0.0f + vTmp[0] * v[2] - sinAngle * v[1], 0.0f,
                        0.0f     + vTmp[1] * v[0] - sinAngle * v[2], cosAngle + vTmp[1] * v[1]              , 0.0f + vTmp[1] * v[2] + sinAngle * v[0], 0.0f,
                        0.0f     + vTmp[2] * v[0] + sinAngle * v[1], 0.0f + vTmp[2] * v[1] - sinAngle * v[0], cosAngle + vTmp[2] * v[2]              , 0.0f,
                        0.0f                                       , 0.0f                                   , 0.0f                                   , 1.0f);

    Mat<Type,4> result;
    result[0] = rotMat[0] * m[0][0] + rotMat[1] * m[0][1] + rotMat[2] * m[0][2];
    result[1] = rotMat[0] * m[1][0] + rotMat[1] * m[1][1] + rotMat[2] * m[1][2];
    result[2] = rotMat[0] * m[2][0] + rotMat[1] * m[2][1] + rotMat[2] * m[2][2];
    result[3] = m[3];

    return result;
  }

  template<typename Type> void decomposeMatrix(Mat<Type,4> matrix, Vec<Type,3>& angle, Vec<Type,3>& position, Vec<Type,3>& scale)
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

    Mat<float,4> rtxMatrix, rtyMatrix, rtzMatrix;
    rtxMatrix = rtyMatrix = rtzMatrix = Mat<float,4>::identity();

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

  #if defined(SSE4)
  template<typename Type> __forceinline Vec<Type,4> operator * ( Vec<Type,4>& v, Mat<Type,4>& m )//SSE4.1
  {
    __m128 a,b,c;
    Vec<Type,4> vTmp, vErg;
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

  template<typename Type> __forceinline Vec<Type,3> operator * ( Vec<Type,3>& v, Mat<Type,3>& m )//SSE4.1
  {
    __m128 a,b,c;
    Vec<Type,3> vTmp, vErg;
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

  template<typename Type> __forceinline Vec<Type,2> operator * ( Vec<Type,2>& v, Mat<Type,2>& m )//SSE4.1
  {
    __m128 a,b,c;
    Vec<Type,2> vTmp, vErg;
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
#endif
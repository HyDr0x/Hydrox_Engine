#ifndef QUATERNION_HPP_
#define QUATERNION_HPP_

#include "Hydrox/Utility/Math/Matrix.hpp"

template<typename TYPE> class Quaternion
{
public:

  Quaternion()
  {}

  Quaternion(Vec<TYPE, 4> v) : m_coord(v)
  {}

  Quaternion(TYPE w, TYPE x, TYPE y, TYPE z) : m_coord(w, x, y, z)
  {}
  ~Quaternion(){}

  static inline Quaternion identity()
  {
    return Quaternion<TYPE>(1.0f, 0.0f, 0.0f, 0.0f);
  }

  Mat<TYPE, 4> toMatrix()
  {
    TYPE xx = m_coord[1] * m_coord[1];
    TYPE xy = m_coord[1] * m_coord[2];
    TYPE xz = m_coord[1] * m_coord[3];
    TYPE xw = m_coord[1] * m_coord[0];

    TYPE yy = m_coord[2] * m_coord[2];
    TYPE yz = m_coord[2] * m_coord[3];
    TYPE yw = m_coord[2] * m_coord[0];

    TYPE zz = m_coord[3] * m_coord[3];
    TYPE zw = m_coord[3] * m_coord[0];

    return Mat<TYPE, 4>(
      1.0f - 2.0f * (yy + zz), 2.0f * (xy + zw), 2.0f * (xz - yw), 0.0f,
      2.0f * (xy - zw), 1.0f - 2.0f * (xx + zz), 2.0f * (yz + xw), 0.0f,
      2.0f * (xz + yw), 2.0f * (yz - xw), 1.0f - 2.0f * (xx + yy), 0.0f,
                  0.0f,             0.0f,                    0.0f, 1.0f
      );
  }

  inline Quaternion conjugate()
  {
    return Quaternion(m_coord[0], -m_coord[1], -m_coord[2], -m_coord[3]);
  }

  inline TYPE length()
  {
    return sqrt(m_coord[0] * m_coord[0] + m_coord[1] * m_coord[1] + m_coord[2] * m_coord[2] + m_coord[3] * m_coord[3]);
  }

  inline TYPE quadLength()
  {
    return m_coord[0] * m_coord[0] + m_coord[1] * m_coord[1] + m_coord[2] * m_coord[2] + m_coord[3] * m_coord[3];
  }

  inline void normalize()
  {
    *this /= length();
  }

  inline Quaternion invert()
  {
    return conjugate() / quadLength();
  }

  inline Quaternion operator + (const Quaternion& q)
  {
    return Quaternion(m_coord + q.m_coord);
  }

  inline Quaternion operator - (const Quaternion& q)
  {
    return Quaternion(m_coord - q.m_coord);
  }

  inline const Quaternion& operator += (const Quaternion& q)
  {
    m_coord += q.m_coord;

    return *this;
  }

  inline const Quaternion& operator -= (const Quaternion& q)
  {
    m_coord -= q.m_coord;

    return *this;
  }

  inline Quaternion operator * (Quaternion& q)
  {
    return Quaternion(  q.m_coord[0] * m_coord[0]
                      - q.m_coord[1] * m_coord[1]
                      - q.m_coord[2] * m_coord[2]
                      - q.m_coord[3] * m_coord[3],
                        q.m_coord[0] * m_coord[1]
                      + q.m_coord[1] * m_coord[0]
                      + q.m_coord[2] * m_coord[3]
                      - q.m_coord[3] * m_coord[2],
                        q.m_coord[0] * m_coord[2]
                      - q.m_coord[1] * m_coord[3]
                      + q.m_coord[2] * m_coord[0]
                      + q.m_coord[3] * m_coord[1],
                        q.m_coord[0] * m_coord[3]
                      + q.m_coord[1] * m_coord[2]
                      - q.m_coord[2] * m_coord[1]
                      + q.m_coord[3] * m_coord[0]);
  }

  inline const Quaternion& operator *= (Quaternion& q)
  {
    Vec<TYPE, 4> tmpErg;

    tmpErg[0] =   q.m_coord[0] * m_coord[0]
                - q.m_coord[1] * m_coord[1]
                - q.m_coord[2] * m_coord[2]
                - q.m_coord[3] * m_coord[3];
    tmpErg[1] =   q.m_coord[0] * m_coord[1]
                + q.m_coord[1] * m_coord[0]
                + q.m_coord[2] * m_coord[3]
                - q.m_coord[3] * m_coord[2];
    tmpErg[2] =   q.m_coord[0] * m_coord[2]
                - q.m_coord[1] * m_coord[3]
                + q.m_coord[2] * m_coord[0]
                + q.m_coord[3] * m_coord[1];
    tmpErg[3] =   q.m_coord[0] * m_coord[3]
                + q.m_coord[1] * m_coord[2]
                - q.m_coord[2] * m_coord[1]
                + q.m_coord[3] * m_coord[0];

    m_coord = tmpErg;

    return *this;
  }

  inline Quaternion operator * (TYPE s)
  {
    return Quaternion(s * m_coord[0], s * m_coord[1], s * m_coord[2], s * m_coord[3]);
  }

  inline const Quaternion& operator *= (TYPE s)
  {
    m_coord[0] *= s;
    m_coord[1] *= s;
    m_coord[2] *= s;
    m_coord[3] *= s;

    return *this;
  }

  inline Quaternion operator / (TYPE s)
  {
    return Quaternion(m_coord[0] / s, m_coord[1] / s, m_coord[2] / s, m_coord[3] / s);
  }

  inline const Quaternion& operator /= (TYPE s)
  {
    m_coord[0] /= s;
    m_coord[1] /= s;
    m_coord[2] /= s;
    m_coord[3] /= s;

    return *this;
  }

  /*inline Vec<TYPE, 3> apply(Vec<TYPE, 3> v)
  {
    TYPE a00 = m_coord[0] * m_coord[0];
    TYPE a01 = m_coord[0] * m_coord[1];
    TYPE a02 = m_coord[0] * m_coord[2];
    TYPE a03 = m_coord[0] * m_coord[3];

    TYPE a11 = m_coord[1] * m_coord[1];
    TYPE a12 = m_coord[1] * m_coord[2];
    TYPE a13 = m_coord[1] * m_coord[3];

    TYPE a22 = m_coord[2] * m_coord[2];
    TYPE a23 = m_coord[2] * m_coord[3];
    TYPE a33 = m_coord[3] * m_coord[3];

    return Vec<TYPE, 3>(v[0] * (a00 + a11 - a22 - a33) + 2.0f * (a12 * v[1] + a13 * v[2] + a02 * v[2] - a03 * v[1]),
                        v[1] * (a00 - a11 + a22 - a33) + 2.0f * (a12 * v[0] + a23 * v[2] + a03 * v[0] - a01 * v[2]),
                        v[2] * (a00 - a11 - a22 + a33) + 2.0f * (a13 * v[0] + a23 * v[1] - a02 * v[0] + a01 * v[1]));
  }*/

  inline Vec<TYPE, 3> apply(Vec<TYPE, 3> v)
  {
    Vec<TYPE, 3> pureQuad = Vec<TYPE, 3>(m_coord[1], m_coord[2], m_coord[3]);
    Vec<TYPE, 3> t = math::cross<TYPE>(v, pureQuad) * 2.0f;
    return v + t * m_coord[0] + math::cross<TYPE>(t, pureQuad);
  }

  static inline Quaternion<TYPE> slerp(Quaternion<TYPE> a, Quaternion<TYPE> b, TYPE t)
  {
    float alpha = acosf(Vec<TYPE, 3>::dot(Vec<TYPE, 3>(a.m_coord[1], a.m_coord[2], a.m_coord[3]), Vec<TYPE, 3>(b.m_coord[1], b.m_coord[2], b.m_coord[3])));

    float sinAlpha = 1.0f / sinf(alpha);

    Quaternion<float> result = a * (sinf(1.0f - t) * alpha) * sinAlpha + b * (sin(t) * alpha) * sinAlpha;
    result.normalize();

    return result;
  }

  static inline Quaternion<TYPE> nlerp(Quaternion<TYPE> a, Quaternion<TYPE> b, TYPE t)
  {
    Quaternion<TYPE> result = (a * (1.0f - t) + b * t);
    result.normalize();//quaternion needs to be normailzed after linear interpolation

    return result;
  }

private:

  Vec<TYPE, 4> m_coord;
};

#endif
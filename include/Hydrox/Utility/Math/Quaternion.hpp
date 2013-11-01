#ifndef QUATERNION_HPP_
#define QUATERNION_HPP_

#include "Hydrox/Utility/Math/math.hpp"

template<typename TYPE> class Quaternion
{
public:

  Quaternion(Vec<TYPE, 4> v) :  : m_coord(v)
  {}

  Quaternion(TYPE w, TYPE x, TYPE y, TYPE z) : m_coord(w, x, y, z)
  {}
  ~Quaternion{}

  Mat<TYPE, 3> toMatrix()
  {
    return Mat<TYPE, 3>(
      1.0f - 2.0f * m_coord[1] * m_coord[1] - 2.0f * m_coord[2] * m_coord[2], 2.0f * m_coord[0] * m_coord[1] - 2.0f * m_coord[2] * m_coord[3], 2.0f * m_coord[0] * m_coord[2] + 2.0f * m_coord[1] * m_coord[3],
      2.0f * m_coord[0] * m_coord[1] + 2.0f * m_coord[2] * m_coord[3], 1.0f - 2.0f * m_coord[0] * m_coord[0] - 2.0f * m_coord[2] * m_coord[2], 2.0f * m_coord[1] * m_coord[2] - 2.0f * m_coord[0] * m_coord[3],
      2.0f * m_coord[0] * m_coord[2] - 2.0f * m_coord[1] * m_coord[3], 2.0f * m_coord[1] * m_coord[2] + 2.0f * m_coord[0] * m_coord[3], 1.0f - 2.0f * m_coord[0] * m_coord[0] - 2.0f * m_coord[1] * m_coord[1]
      );
  }

  inline Quaternion conjugate()
  {
    return Quaternion(m_coord[0], -m_coord[1], -m_coord[2], -m_coord[3]);
  }

  inline float norm()
  {
    return sqrt(m_coord[0] * m_coord[0] + m_coord[1] * -m_coord[1] + m_coord[2] * -m_coord[2] + m_coord[3] * -m_coord[3]);
  }

  inline Quaternion invert()
  {
    float quadnorm = m_coord[0] * m_coord[0] + m_coord[1] * -m_coord[1] + m_coord[2] * -m_coord[2] + m_coord[3] * -m_coord[3];
    return conjugate() / quadnorm;
  }

  inline Quaternion operator + (const Quaternion& q)
  {
    return Quaternion(m_coord + q);
  }

  inline Quaternion operator - (const Quaternion& q)
  {
    return Quaternion(m_coord - q);
  }

  inline const Quaternion& operator += (const Quaternion& q)
  {
    m_coord += q;

    return *this;
  }

  inline const Quaternion& operator -= (const Quaternion& q)
  {
    m_coord -= q;

    return *this;
  }

  inline Quaternion operator * (const Quaternion& q)
  {
    return Quaternion(  m_coord[0] * q.m_coord[0]
                      - m_coord[1] * q.m_coord[1]
                      - m_coord[2] * q.m_coord[2]
                      - m_coord[3] * q.m_coord[3],
                        m_coord[0] * q.m_coord[1]
                      + m_coord[1] * q.m_coord[0]
                      + m_coord[2] * q.m_coord[3]
                      - m_coord[3] * q.m_coord[2],
                        m_coord[0] * q.m_coord[2]
                      - m_coord[1] * q.m_coord[3]
                      + m_coord[2] * q.m_coord[0]
                      + m_coord[3] * q.m_coord[1],
                        m_coord[0] * q.m_coord[3]
                      + m_coord[1] * q.m_coord[2]
                      - m_coord[2] * q.m_coord[1]
                      + m_coord[3] * q.m_coord[0]);
  }

  inline const Quaternion& operator *= (const Quaternion& q)
  {
    m_coord[0] =  m_coord[0] * q.m_coord[0]
                - m_coord[1] * q.m_coord[1]
                - m_coord[2] * q.m_coord[2]
                - m_coord[3] * q.m_coord[3];
    m_coord[1] =  m_coord[0] * q.m_coord[1]
                + m_coord[1] * q.m_coord[0]
                + m_coord[2] * q.m_coord[3]
                - m_coord[3] * q.m_coord[2];
    m_coord[2] =  m_coord[0] * q.m_coord[2]
                - m_coord[1] * q.m_coord[3]
                + m_coord[2] * q.m_coord[0]
                + m_coord[3] * q.m_coord[1];
    m_coord[3] =  m_coord[0] * q.m_coord[3]
                + m_coord[1] * q.m_coord[2]
                - m_coord[2] * q.m_coord[1]
                + m_coord[3] * q.m_coord[0];

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
    return Quaternion(s / m_coord[0], s / m_coord[1], s / m_coord[2], s / m_coord[3]);
  }

  inline const Quaternion& operator /= (TYPE s)
  {
    m_coord[0] /= s;
    m_coord[1] /= s;
    m_coord[2] /= s;
    m_coord[3] /= s;

    return *this;
  }

private:

  Vec<TYPE, 4> m_coord;
}

#endif
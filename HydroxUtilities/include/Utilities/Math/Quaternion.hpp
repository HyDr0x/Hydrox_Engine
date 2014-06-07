#ifndef QUATERNION_HPP_
#define QUATERNION_HPP_

#include "Utilities/Math/Matrix.hpp"

namespace he
{
  namespace util
  {
    template<typename TYPE> class Quaternion
    {
    public:

      Quaternion()
      {}

      Quaternion(Vector<TYPE, 4> v) : m_coord(v)
      {}

      Quaternion(TYPE w, TYPE x, TYPE y, TYPE z) : m_coord(w, x, y, z)
      {}

      static inline Quaternion identity()
      {
        return Quaternion<TYPE>(1.0f, 0.0f, 0.0f, 0.0f);
      }

      Matrix<TYPE, 4> toMatrix() const
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

        return Matrix<TYPE, 4>(
          1.0f - 2.0f * (yy + zz), 2.0f * (xy + zw), 2.0f * (xz - yw), 0.0f,
          2.0f * (xy - zw), 1.0f - 2.0f * (xx + zz), 2.0f * (yz + xw), 0.0f,
          2.0f * (xz + yw), 2.0f * (yz - xw), 1.0f - 2.0f * (xx + yy), 0.0f,
                      0.0f,             0.0f,                    0.0f, 1.0f
          );
      }

      inline Quaternion conjugate() const
      {
        return Quaternion(m_coord[0], -m_coord[1], -m_coord[2], -m_coord[3]);
      }

      inline TYPE length() const
      {
        return sqrt(m_coord[0] * m_coord[0] + m_coord[1] * m_coord[1] + m_coord[2] * m_coord[2] + m_coord[3] * m_coord[3]);
      }

      inline Quaternion normalize() const
      {
        return *this / length();
      }

      inline Quaternion invert() const
      {
        return conjugate() / dot(*this, *this);
      }

      inline Quaternion operator + (const Quaternion& q) const
      {
        return Quaternion(m_coord + q.m_coord);
      }

      inline Quaternion operator - (const Quaternion& q) const
      {
        return Quaternion(m_coord - q.m_coord);
      }

      inline Quaternion operator * (const Quaternion& q) const
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

      inline Quaternion operator * (TYPE s) const
      {
        return Quaternion(s * m_coord[0], s * m_coord[1], s * m_coord[2], s * m_coord[3]);
      }

      inline Quaternion operator / (TYPE s) const
      {
        return Quaternion(m_coord[0] / s, m_coord[1] / s, m_coord[2] / s, m_coord[3] / s);
      }

      inline Quaternion& operator += (const Quaternion& q)
      {
        m_coord += q.m_coord;

        return *this;
      }

      inline Quaternion& operator -= (const Quaternion& q)
      {
        m_coord -= q.m_coord;

        return *this;
      }

      inline Quaternion& operator *= (const Quaternion& q)
      {
        Vector<TYPE, 4> tmpErg;

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

      inline Quaternion& operator *= (TYPE s)
      {
        m_coord[0] *= s;
        m_coord[1] *= s;
        m_coord[2] *= s;
        m_coord[3] *= s;

        return *this;
      }

      inline Quaternion& operator /= (TYPE s)
      {
        m_coord[0] /= s;
        m_coord[1] /= s;
        m_coord[2] /= s;
        m_coord[3] /= s;

        return *this;
      }

      inline TYPE& operator [] (unsigned int i) 
      { 
        return m_coord[i]; 
      }

      inline const TYPE& operator [] (unsigned int i) const
      { 
        return m_coord[i]; 
      }

      /*inline Vector<TYPE, 3> apply(Vector<TYPE, 3> v)
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

        return Vector<TYPE, 3>(v[0] * (a00 + a11 - a22 - a33) + 2.0f * (a12 * v[1] + a13 * v[2] + a02 * v[2] - a03 * v[1]),
                            v[1] * (a00 - a11 + a22 - a33) + 2.0f * (a12 * v[0] + a23 * v[2] + a03 * v[0] - a01 * v[2]),
                            v[2] * (a00 - a11 - a22 + a33) + 2.0f * (a13 * v[0] + a23 * v[1] - a02 * v[0] + a01 * v[1]));
      }*/

      inline Vector<TYPE, 3> apply(const Vector<TYPE, 3>& v) const
      {
        Vector<TYPE, 3> pureQuad = Vector<TYPE, 3>(m_coord[1], m_coord[2], m_coord[3]);
        Vector<TYPE, 3> t = math::cross<TYPE>(v, pureQuad) * 2.0f;
        return v + t * m_coord[0] + math::cross<TYPE>(t, pureQuad);
      }

      static inline TYPE dot(const Quaternion<TYPE>& a, const Quaternion<TYPE>& b)
      {
        return a.m_coord[0] * b.m_coord[0] + a.m_coord[1] * b.m_coord[1] + a.m_coord[2] * b.m_coord[2] + a.m_coord[3] * b.m_coord[3];
      }

      static inline Quaternion<TYPE> slerp(const Quaternion<TYPE>& a, const Quaternion<TYPE>& b, TYPE t)
      {
        float cosTheta = Quaternion<TYPE>::dot(a, b);
        if(cosTheta > 0.9995f)
        {
          return nlerp(a, b, t);
        }
        else
        {
          float theta = acosf(math::clamp(cosTheta, -1.0f, 1.0f));
          float thetap = theta * t;

          Quaternion<float> qperp = (b - a * cosTheta).normalize();
          //qperp.normalize();

          //Quaternion<float> result = (a * sinf((1.0f - t) * theta) + b * sinf(t * theta)) / sinf(theta);

          return a * cosf(thetap) + qperp * sinf(thetap);
        }
      }

      static inline Quaternion<TYPE> nlerp(const Quaternion<TYPE>& a, const Quaternion<TYPE>& b, TYPE t)
      {
        Quaternion<TYPE> result = (a * (1.0f - t) + b * t);

        return result.normalize();//quaternion needs to be normailzed after linear interpolation
      }

    private:

      Vector<TYPE, 4> m_coord;
    };

    template<typename Type> std::ofstream& operator<<(std::ofstream& stream, const Quaternion<Type>& q)
    {
      for(unsigned int i = 0; i < 4; i++)
      {
        stream << q[i];
        if(i + 1 != 4)
        {
          stream << ',';
        }
      }

      return stream;
    }

    template<typename Type> std::ifstream& operator>>(std::ifstream& stream, Quaternion<Type>& q)
    {
      for(unsigned int i = 0; i < 4; i++)
      {
        stream >> q[i];
        stream.ignore(1, ',');//ignores the next char or  ','
      }

      return stream;
    }
  }
}

#endif

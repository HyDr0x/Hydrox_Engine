#ifndef MATRIX4_H_
#define MATRIX4_H_

namespace he
{
  namespace util
  {
    template<typename Type> class Matrix<Type, 4>
    {
      public:

      Matrix()
      {}

      Matrix(const Matrix<Type, 4>& m)
      {
        m_m[0] = m.m_m[0];
        m_m[1] = m.m_m[1];
        m_m[2] = m.m_m[2];
        m_m[3] = m.m_m[3];
      }

      Matrix(const Vector<Type, 4>& v0, const Vector<Type, 4>& v1, const Vector<Type, 4>& v2, const Vector<Type, 4>& v3 )
      {
        m_m[0][0]=v0[0];  m_m[1][0]=v1[0];  m_m[2][0]=v2[0];  m_m[3][0]=v3[0];
        m_m[0][1]=v0[1];  m_m[1][1]=v1[1];  m_m[2][1]=v2[1];  m_m[3][1]=v3[1];
        m_m[0][2]=v0[2];  m_m[1][2]=v1[2];  m_m[2][2]=v2[2];  m_m[3][2]=v3[2];
        m_m[0][3]=v0[3];  m_m[1][3]=v1[3];  m_m[2][3]=v2[3];  m_m[3][3]=v3[3];
      }

      Matrix(const Type *a)
      {
        for(int i = 0; i < 4; i++)
        {
          for(int j = 0; j < 4; j++)
          {
            m_m[i][j] = a[4 * i + j];
          }
        }
      }

      Matrix(Type m_11,  Type m_21,  Type m_31,  Type m_41,
             Type m_12,  Type m_22,  Type m_32,  Type m_42,
             Type m_13,  Type m_23,  Type m_33,  Type m_43,
             Type m_14,  Type m_24,  Type m_34,  Type m_44)
      {
        m_m[0][0]=m_11;  m_m[1][0]=m_21;  m_m[2][0]=m_31;  m_m[3][0]=m_41;
        m_m[0][1]=m_12;  m_m[1][1]=m_22;  m_m[2][1]=m_32;  m_m[3][1]=m_42;
        m_m[0][2]=m_13;  m_m[1][2]=m_23;  m_m[2][2]=m_33;  m_m[3][2]=m_43;
        m_m[0][3]=m_14;  m_m[1][3]=m_24;  m_m[2][3]=m_34;  m_m[3][3]=m_44;
      }

      static inline Matrix identity()
      {
        return Matrix<Type, 4>(1.0f,  0.0f,  0.0f,  0.0f,
                               0.0f,  1.0f,  0.0f,  0.0f,
                               0.0f,  0.0f,  1.0f,  0.0f,
                               0.0f,  0.0f,  0.0f,  1.0f);
      }

      inline Vector<Type, 4>& operator [](unsigned int i)
      {
        return m_m[i];
      }

      inline const Vector<Type, 4>& operator [](unsigned int i) const
      {
        return m_m[i];
      }

      inline Type detMat() const
      {
        return m_m[0][0] * (m_m[1][1]*m_m[2][2]*m_m[3][3] + m_m[2][1]*m_m[3][2]*m_m[1][3] + m_m[3][1]*m_m[1][2]*m_m[2][3] - m_m[1][1]*m_m[3][2]*m_m[2][3] - m_m[2][1]*m_m[1][2]*m_m[3][3] - m_m[3][1]*m_m[2][2]*m_m[1][3])+
               m_m[1][0] * (m_m[0][1]*m_m[3][2]*m_m[2][3] + m_m[2][1]*m_m[0][2]*m_m[3][3] + m_m[3][1]*m_m[2][2]*m_m[0][3] - m_m[0][1]*m_m[2][2]*m_m[3][3] - m_m[2][1]*m_m[3][2]*m_m[0][3] - m_m[3][1]*m_m[0][2]*m_m[2][3])+
               m_m[2][0] * (m_m[0][1]*m_m[1][2]*m_m[3][3] + m_m[1][1]*m_m[3][2]*m_m[0][3] + m_m[3][1]*m_m[0][2]*m_m[1][3] - m_m[0][1]*m_m[3][2]*m_m[1][3] - m_m[1][1]*m_m[0][2]*m_m[3][3] - m_m[3][1]*m_m[1][2]*m_m[0][3])+
               m_m[3][0] * (m_m[0][1]*m_m[2][2]*m_m[1][3] + m_m[1][1]*m_m[0][2]*m_m[2][3] + m_m[2][1]*m_m[1][2]*m_m[0][3] - m_m[0][1]*m_m[1][2]*m_m[2][3] - m_m[1][1]*m_m[2][2]*m_m[0][3] - m_m[2][1]*m_m[0][2]*m_m[1][3]);
      }

      inline Matrix invert() const
      {
        Type InvDetM = detMat();
        Matrix invMat;

        if(InvDetM)
        {
          InvDetM = 1.0f / InvDetM;

          invMat[0][0] = m_m[1][1]*m_m[2][2]*m_m[3][3] + m_m[2][1]*m_m[3][2]*m_m[1][3] + m_m[3][1]*m_m[1][2]*m_m[2][3] - m_m[1][1]*m_m[3][2]*m_m[2][3] - m_m[2][1]*m_m[1][2]*m_m[3][3] - m_m[3][1]*m_m[2][2]*m_m[1][3];
          invMat[1][0] = m_m[1][0]*m_m[3][2]*m_m[2][3] + m_m[2][0]*m_m[1][2]*m_m[3][3] + m_m[3][0]*m_m[2][2]*m_m[1][3] - m_m[1][0]*m_m[2][2]*m_m[3][3] - m_m[2][0]*m_m[3][2]*m_m[1][3] - m_m[3][0]*m_m[1][2]*m_m[2][3];
          invMat[2][0] = m_m[1][0]*m_m[2][1]*m_m[3][3] + m_m[2][0]*m_m[3][1]*m_m[1][3] + m_m[3][0]*m_m[1][1]*m_m[2][3] - m_m[1][0]*m_m[3][1]*m_m[2][3] - m_m[2][0]*m_m[1][1]*m_m[3][3] - m_m[3][0]*m_m[2][1]*m_m[1][3];
          invMat[3][0] = m_m[1][0]*m_m[3][1]*m_m[2][2] + m_m[2][0]*m_m[1][1]*m_m[3][2] + m_m[3][0]*m_m[2][1]*m_m[1][2] - m_m[1][0]*m_m[2][1]*m_m[3][2] - m_m[2][0]*m_m[3][1]*m_m[1][2] - m_m[3][0]*m_m[1][1]*m_m[2][2];

          invMat[0][1] = m_m[0][1]*m_m[3][2]*m_m[2][3] + m_m[2][1]*m_m[0][2]*m_m[3][3] + m_m[3][1]*m_m[2][2]*m_m[0][3] - m_m[0][1]*m_m[2][2]*m_m[3][3] - m_m[2][1]*m_m[3][2]*m_m[0][3] - m_m[3][1]*m_m[0][2]*m_m[2][3];
          invMat[1][1] = m_m[0][0]*m_m[2][2]*m_m[3][3] + m_m[2][0]*m_m[3][2]*m_m[0][3] + m_m[3][0]*m_m[0][2]*m_m[2][3] - m_m[0][0]*m_m[3][2]*m_m[2][3] - m_m[2][0]*m_m[0][2]*m_m[3][3] - m_m[3][0]*m_m[2][2]*m_m[0][3];
          invMat[2][1] = m_m[0][0]*m_m[3][1]*m_m[2][3] + m_m[2][0]*m_m[0][1]*m_m[3][3] + m_m[3][0]*m_m[2][1]*m_m[0][3] - m_m[0][0]*m_m[2][1]*m_m[3][3] - m_m[2][0]*m_m[3][1]*m_m[0][3] - m_m[3][0]*m_m[0][1]*m_m[2][3];
          invMat[3][1] = m_m[0][0]*m_m[2][1]*m_m[3][2] + m_m[2][0]*m_m[3][1]*m_m[0][2] + m_m[3][0]*m_m[0][1]*m_m[2][2] - m_m[0][0]*m_m[3][1]*m_m[2][2] - m_m[2][0]*m_m[0][1]*m_m[3][2] - m_m[3][0]*m_m[2][1]*m_m[0][2];

          invMat[0][2] = m_m[0][1]*m_m[1][2]*m_m[3][3] + m_m[1][1]*m_m[3][2]*m_m[0][3] + m_m[3][1]*m_m[0][2]*m_m[1][3] - m_m[0][1]*m_m[3][2]*m_m[1][3] - m_m[1][1]*m_m[0][2]*m_m[3][3] - m_m[3][1]*m_m[1][2]*m_m[0][3];
          invMat[1][2] = m_m[0][0]*m_m[3][2]*m_m[1][3] + m_m[1][0]*m_m[0][2]*m_m[3][3] + m_m[3][0]*m_m[1][2]*m_m[0][3] - m_m[0][0]*m_m[1][2]*m_m[3][3] - m_m[1][0]*m_m[3][2]*m_m[0][3] - m_m[3][0]*m_m[0][2]*m_m[1][3];
          invMat[2][2] = m_m[0][0]*m_m[1][1]*m_m[3][3] + m_m[1][0]*m_m[3][1]*m_m[0][3] + m_m[3][0]*m_m[0][1]*m_m[1][3] - m_m[0][0]*m_m[3][1]*m_m[1][3] - m_m[1][0]*m_m[0][1]*m_m[3][3] - m_m[3][0]*m_m[1][1]*m_m[0][3];
          invMat[3][2] = m_m[0][0]*m_m[3][1]*m_m[1][2] + m_m[1][0]*m_m[0][1]*m_m[3][2] + m_m[3][0]*m_m[1][1]*m_m[0][2] - m_m[0][0]*m_m[1][1]*m_m[3][2] - m_m[1][0]*m_m[3][1]*m_m[0][2] - m_m[3][0]*m_m[0][1]*m_m[1][2];

          invMat[0][3] = m_m[0][1]*m_m[2][2]*m_m[1][3] + m_m[1][1]*m_m[0][2]*m_m[2][3] + m_m[2][1]*m_m[1][2]*m_m[0][3] - m_m[0][1]*m_m[1][2]*m_m[2][3] - m_m[1][1]*m_m[2][2]*m_m[0][3] - m_m[2][1]*m_m[0][2]*m_m[1][3];
          invMat[1][3] = m_m[0][0]*m_m[1][2]*m_m[2][3] + m_m[1][0]*m_m[2][2]*m_m[0][3] + m_m[2][0]*m_m[0][2]*m_m[1][3] - m_m[0][0]*m_m[2][2]*m_m[1][3] - m_m[1][0]*m_m[0][2]*m_m[2][3] - m_m[2][0]*m_m[1][2]*m_m[0][3];
          invMat[2][3] = m_m[0][0]*m_m[2][1]*m_m[1][3] + m_m[1][0]*m_m[0][1]*m_m[2][3] + m_m[2][0]*m_m[1][1]*m_m[0][3] - m_m[0][0]*m_m[1][1]*m_m[2][3] - m_m[1][0]*m_m[2][1]*m_m[0][3] - m_m[2][0]*m_m[0][1]*m_m[1][3];
          invMat[3][3] = m_m[0][0]*m_m[1][1]*m_m[2][2] + m_m[1][0]*m_m[2][1]*m_m[0][2] + m_m[2][0]*m_m[0][1]*m_m[1][2] - m_m[0][0]*m_m[2][1]*m_m[1][2] - m_m[1][0]*m_m[0][1]*m_m[2][2] - m_m[2][0]*m_m[1][1]*m_m[0][2];

          invMat *= InvDetM;
        }

        return invMat;
      }

      inline Matrix transpose() const
      {
        return Matrix<Type, 4>(m_m[0][0], m_m[0][1], m_m[0][2], m_m[0][3],
                               m_m[1][0], m_m[1][1], m_m[1][2], m_m[1][3],
                               m_m[2][0], m_m[2][1], m_m[2][2], m_m[2][3],
                               m_m[3][0], m_m[3][1], m_m[3][2], m_m[3][3]);
      }

      inline Matrix operator + (const Matrix& m) const
      {
        Matrix erg;

        erg[0] = m_m[0] + m[0];
        erg[1] = m_m[1] + m[1];
        erg[2] = m_m[2] + m[2];
        erg[3] = m_m[3] + m[3];

        return erg;
      }

      inline Matrix operator - (const Matrix& m) const
      {
        Matrix erg;

        erg[0] = m_m[0] - m[0];
        erg[1] = m_m[1] - m[1];
        erg[2] = m_m[2] - m[2];
        erg[3] = m_m[3] - m[3];

        return erg;
      }

      inline Matrix operator * (const Matrix<Type, 4>& m) const
      {
        Matrix<Type, 4> erg;

        erg[0][0] = m_m[0][0] * m[0][0] + m_m[1][0] * m[0][1] + m_m[2][0] * m[0][2] + m_m[3][0] * m[0][3];
        erg[0][1] = m_m[0][1] * m[0][0] + m_m[1][1] * m[0][1] + m_m[2][1] * m[0][2] + m_m[3][1] * m[0][3];
        erg[0][2] = m_m[0][2] * m[0][0] + m_m[1][2] * m[0][1] + m_m[2][2] * m[0][2] + m_m[3][2] * m[0][3];
        erg[0][3] = m_m[0][3] * m[0][0] + m_m[1][3] * m[0][1] + m_m[2][3] * m[0][2] + m_m[3][3] * m[0][3];

        erg[1][0] = m_m[0][0] * m[1][0] + m_m[1][0] * m[1][1] + m_m[2][0] * m[1][2] + m_m[3][0] * m[1][3];
        erg[1][1] = m_m[0][1] * m[1][0] + m_m[1][1] * m[1][1] + m_m[2][1] * m[1][2] + m_m[3][1] * m[1][3];
        erg[1][2] = m_m[0][2] * m[1][0] + m_m[1][2] * m[1][1] + m_m[2][2] * m[1][2] + m_m[3][2] * m[1][3];
        erg[1][3] = m_m[0][3] * m[1][0] + m_m[1][3] * m[1][1] + m_m[2][3] * m[1][2] + m_m[3][3] * m[1][3];

        erg[2][0] = m_m[0][0] * m[2][0] + m_m[1][0] * m[2][1] + m_m[2][0] * m[2][2] + m_m[3][0] * m[2][3];
        erg[2][1] = m_m[0][1] * m[2][0] + m_m[1][1] * m[2][1] + m_m[2][1] * m[2][2] + m_m[3][1] * m[2][3];
        erg[2][2] = m_m[0][2] * m[2][0] + m_m[1][2] * m[2][1] + m_m[2][2] * m[2][2] + m_m[3][2] * m[2][3];
        erg[2][3] = m_m[0][3] * m[2][0] + m_m[1][3] * m[2][1] + m_m[2][3] * m[2][2] + m_m[3][3] * m[2][3];

        erg[3][0] = m_m[0][0] * m[3][0] + m_m[1][0] * m[3][1] + m_m[2][0] * m[3][2] + m_m[3][0] * m[3][3];
        erg[3][1] = m_m[0][1] * m[3][0] + m_m[1][1] * m[3][1] + m_m[2][1] * m[3][2] + m_m[3][1] * m[3][3];
        erg[3][2] = m_m[0][2] * m[3][0] + m_m[1][2] * m[3][1] + m_m[2][2] * m[3][2] + m_m[3][2] * m[3][3];
        erg[3][3] = m_m[0][3] * m[3][0] + m_m[1][3] * m[3][1] + m_m[2][3] * m[3][2] + m_m[3][3] * m[3][3];

        return erg;
      }

      inline Matrix operator * (Type s) const
      {
        Matrix erg;

        erg[0] = m_m[0] * s;
        erg[1] = m_m[1] * s;
        erg[2] = m_m[2] * s;
        erg[3] = m_m[3] * s;

        return erg;
      }

      inline Vector<Type, 4> operator * (const Vector<Type, 4>& v) const
      {
        Vector<Type, 4> vErg;

        vErg[0] = m_m[0][0] * v[0] + m_m[1][0] * v[1] + m_m[2][0] * v[2] + m_m[3][0] * v[3];
        vErg[1] = m_m[0][1] * v[0] + m_m[1][1] * v[1] + m_m[2][1] * v[2] + m_m[3][1] * v[3];
        vErg[2] = m_m[0][2] * v[0] + m_m[1][2] * v[1] + m_m[2][2] * v[2] + m_m[3][2] * v[3];
        vErg[3] = m_m[0][3] * v[0] + m_m[1][3] * v[1] + m_m[2][3] * v[2] + m_m[3][3] * v[3];

        return vErg;
      }

      inline Matrix& operator += (const Matrix& m)
      {
        m_m[0] += m[0];
        m_m[1] += m[1];
        m_m[2] += m[2];
        m_m[3] += m[3];

        return *this;
      }

      inline Matrix& operator -= (const Matrix& m)
      {
        m_m[0] -= m[0];
        m_m[1] -= m[1];
        m_m[2] -= m[2];
        m_m[3] -= m[3];

        return *this;
      }

      inline Matrix<Type, 4>& operator *= (const Matrix& m)
      {
        Matrix<Type, 4> tmpMat = *this;

        m_m[0][0] = tmpMat[0][0] * m[0][0] + tmpMat[1][0] * m[0][1] + tmpMat[2][0] * m[0][2] + tmpMat[3][0] * m[0][3];
        m_m[0][1] = tmpMat[0][1] * m[0][0] + tmpMat[1][1] * m[0][1] + tmpMat[2][1] * m[0][2] + tmpMat[3][1] * m[0][3];
        m_m[0][2] = tmpMat[0][2] * m[0][0] + tmpMat[1][2] * m[0][1] + tmpMat[2][2] * m[0][2] + tmpMat[3][2] * m[0][3];
        m_m[0][3] = tmpMat[0][3] * m[0][0] + tmpMat[1][3] * m[0][1] + tmpMat[2][3] * m[0][2] + tmpMat[3][3] * m[0][3];

        m_m[1][0] = tmpMat[0][0] * m[1][0] + tmpMat[1][0] * m[1][1] + tmpMat[2][0] * m[1][2] + tmpMat[3][0] * m[1][3];
        m_m[1][1] = tmpMat[0][1] * m[1][0] + tmpMat[1][1] * m[1][1] + tmpMat[2][1] * m[1][2] + tmpMat[3][1] * m[1][3];
        m_m[1][2] = tmpMat[0][2] * m[1][0] + tmpMat[1][2] * m[1][1] + tmpMat[2][2] * m[1][2] + tmpMat[3][2] * m[1][3];
        m_m[1][3] = tmpMat[0][3] * m[1][0] + tmpMat[1][3] * m[1][1] + tmpMat[2][3] * m[1][2] + tmpMat[3][3] * m[1][3];

        m_m[2][0] = tmpMat[0][0] * m[2][0] + tmpMat[1][0] * m[2][1] + tmpMat[2][0] * m[2][2] + tmpMat[3][0] * m[2][3];
        m_m[2][1] = tmpMat[0][1] * m[2][0] + tmpMat[1][1] * m[2][1] + tmpMat[2][1] * m[2][2] + tmpMat[3][1] * m[2][3];
        m_m[2][2] = tmpMat[0][2] * m[2][0] + tmpMat[1][2] * m[2][1] + tmpMat[2][2] * m[2][2] + tmpMat[3][2] * m[2][3];
        m_m[2][3] = tmpMat[0][3] * m[2][0] + tmpMat[1][3] * m[2][1] + tmpMat[2][3] * m[2][2] + tmpMat[3][3] * m[2][3];

        m_m[3][0] = tmpMat[0][0] * m[3][0] + tmpMat[1][0] * m[3][1] + tmpMat[2][0] * m[3][2] + tmpMat[3][0] * m[3][3];
        m_m[3][1] = tmpMat[0][1] * m[3][0] + tmpMat[1][1] * m[3][1] + tmpMat[2][1] * m[3][2] + tmpMat[3][1] * m[3][3];
        m_m[3][2] = tmpMat[0][2] * m[3][0] + tmpMat[1][2] * m[3][1] + tmpMat[2][2] * m[3][2] + tmpMat[3][2] * m[3][3];
        m_m[3][3] = tmpMat[0][3] * m[3][0] + tmpMat[1][3] * m[3][1] + tmpMat[2][3] * m[3][2] + tmpMat[3][3] * m[3][3];

        return *this;
      }

      inline Matrix& operator *= (Type s)
      {
        m_m[0] *= s;
        m_m[1] *= s;
        m_m[2] *= s;
        m_m[3] *= s;

        return *this;
      }

      private:

      Vector<Type, 4> m_m[4];
    };

    template<typename Type> std::ostream& operator<<(std::ostream& stream, const Matrix<Type, 4>& matrix)
    {
      for(unsigned int i = 0; i < 4; i++)
      {
        stream << matrix[i] << std::endl;
      }

      return stream;
    }

    template<typename Type> std::istream& operator>>(std::istream& stream, Matrix<Type, 4>& matrix)
    {
      for(unsigned int i = 0; i < 4; i++)
      {
        stream >> matrix[i];
      }

      return stream;
    }
  }
}

#endif

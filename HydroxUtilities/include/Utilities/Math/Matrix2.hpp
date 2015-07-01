#ifndef MATRIX2_H_
#define MATRIX2_H_

namespace he
{
  namespace util
  {
    template<typename Type> class Matrix<Type, 2>
    {
      public:

      Matrix()
      {}

      Matrix(const Matrix<Type, 2>& m)
      {
        m_m[0] = m.m_m[0];
        m_m[1] = m.m_m[1];
      }

      Matrix(const Vector<Type, 2>& v0, const Vector<Type, 2>& v1)
      {
        m_m[0][0]=v0[0];  m_m[1][0]=v0[1];
        m_m[0][1]=v1[0];  m_m[1][1]=v1[1];
      }

      Matrix(const Type *a)
      {
        for(int i = 0; i < 2; i++)
        {
          for(int j = 0; j < 2; j++)
          {
            m_m[i][j] = a[4 * i + j];
          }
        }
      }

      Matrix(Type m_11,  Type m_21,
             Type m_12,  Type m_22)
      {
        m_m[0][0]=m_11;  m_m[1][0]=m_21;
        m_m[0][1]=m_12;  m_m[1][1]=m_22;
      }

      static inline Matrix identity()
      {
        return Matrix<Type, 2>(1.0f,  0.0f,
                               0.0f,  1.0f);
      }

      inline Vector<Type, 2>& operator [](unsigned int i)
      {
        return m_m[i];
      }

      inline const Vector<Type, 2>& operator [](unsigned int i) const
      {
        return m_m[i];
      }

      inline Type detMat() const
      {
        return m_m[0][0] * m_m[1][1] - m_m[1][0] * m_m[0][1];
      }

      inline Matrix invert() const
      {
        Type invDetM = detMat();
        Matrix invM;

        if(invDetM)
        {
          invDetM = 1.0f / invDetM;

          invM[0][0] = m_m[1][1];
          invM[1][0] =-m_m[0][1];
          invM[0][1] =-m_m[1][0];
          invM[1][1] = m_m[0][0];

          invM *= invDetM;
        }

        return invM;
      }

      inline Matrix transpose() const
      {
        return Matrix<Type, 2>(m_m[0][0], m_m[0][1],
                               m_m[1][0], m_m[1][1]);
      }

      inline Matrix operator + (const Matrix& m) const
      {
        Matrix result;

        result[0] = m_m[0] + m[0];
        result[1] = m_m[1] + m[1];

        return result;
      }

      inline Matrix operator - (const Matrix& m) const
      {
        Matrix result;

        result[0] = m_m[0] - m[0];
        result[1] = m_m[1] - m[1];

        return result;
      }

      inline Matrix operator * (const Matrix<Type, 2>& m) const
      {
        Matrix<Type, 2> result;

        result[0][0] = m_m[0][0] * m[0][0] + m_m[1][0] * m[0][1];
        result[0][1] = m_m[0][1] * m[0][0] + m_m[1][1] * m[0][1];

        result[1][0] = m_m[0][0] * m[1][0] + m_m[1][0] * m[1][1];
        result[1][1] = m_m[0][1] * m[1][0] + m_m[1][1] * m[1][1];

        return result;
      }

      inline Matrix operator * (Type s) const
      {
        Matrix result;

        result[0] = m_m[0] * s;
        result[1] = m_m[1] * s;

        return result;
      }

      inline Vector<Type, 2> operator * (const Vector<Type,2>& v) const
      {
        Vector<Type, 2> vErg;

        vErg[0] = m_m[0][0] * v[0] + m_m[1][0] * v[1];
        vErg[1] = m_m[0][1] * v[0] + m_m[1][1] * v[1];

        return vErg;
      }

      inline Matrix& operator += (const Matrix& m)
      {
        m_m[0] += m[0];
        m_m[1] += m[1];

        return *this;
      }

      inline Matrix& operator -= (const Matrix& m)
      {
        m_m[0] -= m[0];
        m_m[1] -= m[1];

        return *this;
      }

      inline Matrix& operator *= (const Matrix<Type, 2>& m)
      {
        Matrix<Type, 2> tmpMat = *this;

        m_m[0][0] = tmpMat[0][0] * m[0][0] + tmpMat[1][0] * m[0][1];
        m_m[0][1] = tmpMat[0][1] * m[0][0] + tmpMat[1][1] * m[0][1];

        m_m[1][0] = tmpMat[0][0] * m[1][0] + tmpMat[1][0] * m[1][1];
        m_m[1][1] = tmpMat[0][1] * m[1][0] + tmpMat[1][1] * m[1][1];

        return *this;
      }

      inline Matrix& operator *= (Type s)
      {
        m_m[0] *= s;
        m_m[1] *= s;

        return *this;
      }

    private:

      Vector<Type, 2> m_m[2];
    };

    template<typename Type> std::ostream& operator<<(std::ostream& stream, const Matrix<Type, 2>& matrix)
    {
      for(unsigned int i = 0; i < 2; i++)
      {
        stream << matrix[i] << std::endl;;
      }

      return stream;
    }

    template<typename Type> std::istream& operator>>(std::istream& stream, Matrix<Type, 2>& matrix)
    {
      for(unsigned int i = 0; i < 2; i++)
      {
        stream >> matrix[i];
      }

      return stream;
    }
  }
}

#endif

#ifndef MATRIX3_H_
#define MATRIX3_H_

namespace he
{
	namespace util
	{
    template<typename Type> class Matrix<Type, 3>
    {
      public:

      Matrix()
      {}

      Matrix(const Matrix<Type, 3>& m)
      {
        m_m[0] = m.m_m[0];
        m_m[1] = m.m_m[1];
        m_m[2] = m.m_m[2];
      }


      Matrix(const Vector<Type, 3>& v0, const Vector<Type, 3>& v1, const Vector<Type, 3>& v2)
      {
        m_m[0][0]=v0[0]; m_m[1][0]=v1[0]; m_m[2][0]=v2[0];
        m_m[0][1]=v0[1]; m_m[1][1]=v1[1]; m_m[2][1]=v2[1];
        m_m[0][2]=v0[2]; m_m[1][2]=v1[2]; m_m[2][2]=v2[2];
      }

      Matrix(const Type *a)
      {
        for(int i = 0; i < 3; i++)
        {
          for(int j = 0; j < 3; j++)
          {
            m_m[i][j] = a[4 * i + j];
          }
        }
      }

      Matrix(Type m_11, Type m_21, Type m_31,
             Type m_12, Type m_22, Type m_32,
             Type m_13, Type m_23, Type m_33)
      {
        m_m[0][0]=m_11; m_m[1][0]=m_21; m_m[2][0]=m_31;
        m_m[0][1]=m_12; m_m[1][1]=m_22; m_m[2][1]=m_32;
        m_m[0][2]=m_13; m_m[1][2]=m_23; m_m[2][2]=m_33;
      }

      static inline Matrix identity() 
      {
        return Matrix<Type, 3>(1.0f,  0.0f,  0.0f,
                               0.0f,  1.0f,  0.0f,
                               0.0f,  0.0f,  1.0f);
      }

      inline Vector<Type, 3>& operator [](unsigned int i)
	    {
	      return m_m[i];
	    }

      inline const Vector<Type, 3>& operator [](unsigned int i) const
	    {
	      return m_m[i];
	    }

	    inline Type detMat() const
	    {
	      return m_m[0][0]*(m_m[1][1]*m_m[2][2]-m_m[2][1]*m_m[1][2])+
               m_m[0][1]*(m_m[1][2]*m_m[2][0]-m_m[2][2]*m_m[1][0])+
               m_m[0][2]*(m_m[1][0]*m_m[2][1]-m_m[1][1]*m_m[2][0]);
	    }

	    inline Matrix invert() const
	    {
		    Type invDetM = detMat();
		    Matrix invM;

		    if(invDetM)
		    {
			    invDetM = 1.0f / invDetM;
			
          invM[0][0] =  m_m[1][1] * m_m[2][2] - m_m[1][2] * m_m[2][1];
          invM[0][1] =-(m_m[0][1] * m_m[2][2] - m_m[0][2] * m_m[2][1]);
          invM[0][2] =  m_m[0][1] * m_m[1][2] - m_m[0][2] * m_m[1][1];
          invM[1][0] =-(m_m[1][0] * m_m[2][2] - m_m[1][2] * m_m[2][0]); 
          invM[1][1] =  m_m[0][0] * m_m[2][2] - m_m[0][2] * m_m[2][0]; 
          invM[1][2] =-(m_m[0][0] * m_m[1][2] - m_m[0][2] * m_m[1][0]);
          invM[2][0] =  m_m[1][0] * m_m[2][1] - m_m[1][1] * m_m[2][0]; 
          invM[2][1] =-(m_m[0][0] * m_m[2][1] - m_m[0][1] * m_m[2][0]); 
          invM[2][2] =  m_m[0][0] * m_m[1][1] - m_m[0][1] * m_m[1][0];

          invM *= invDetM;
		    }

		    return invM;
	    }

      inline Matrix transpose() const
      {
        return Matrix<Type, 3>(m_m[0][0], m_m[0][1], m_m[0][2],
                               m_m[1][0], m_m[1][1], m_m[1][2],
                               m_m[2][0], m_m[2][1], m_m[2][2]);
      }

      inline Matrix operator + (const Matrix& m) const
      {
        Matrix result;

        result[0] = m_m[0] + m[0];
        result[1] = m_m[1] + m[1];
        result[2] = m_m[2] + m[2];

        return result;
      }

      inline Matrix operator - (const Matrix& m) const
      {
		    Matrix result;

        result[0] = m_m[0] - m[0];
        result[1] = m_m[1] - m[1];
        result[2] = m_m[2] - m[2];

        return result;
      }

      inline Matrix operator * (const Matrix<Type, 3>& m) const
      {
        Matrix result;

        result[0][0] = m_m[0][0] * m[0][0] + m_m[1][0] * m[0][1] + m_m[2][0] * m[0][2];
        result[0][1] = m_m[0][1] * m[0][0] + m_m[1][1] * m[0][1] + m_m[2][1] * m[0][2];
        result[0][2] = m_m[0][2] * m[0][0] + m_m[1][2] * m[0][1] + m_m[2][2] * m[0][2];

        result[1][0] = m_m[0][0] * m[1][0] + m_m[1][0] * m[1][1] + m_m[2][0] * m[1][2];
        result[1][1] = m_m[0][1] * m[1][0] + m_m[1][1] * m[1][1] + m_m[2][1] * m[1][2];
        result[1][2] = m_m[0][2] * m[1][0] + m_m[1][2] * m[1][1] + m_m[2][2] * m[1][2];

        result[2][0] = m_m[0][0] * m[2][0] + m_m[1][0] * m[2][1] + m_m[2][0] * m[2][2];
        result[2][1] = m_m[0][1] * m[2][0] + m_m[1][1] * m[2][1] + m_m[2][1] * m[2][2];
        result[2][2] = m_m[0][2] * m[2][0] + m_m[1][2] * m[2][1] + m_m[2][2] * m[2][2];

        return result;
      }

      inline Matrix operator * (Type s) const
      {
        Matrix result;

        result[0] = m_m[0] * s;
        result[1] = m_m[1] * s;
        result[2] = m_m[2] * s;

        return result;
      }

      inline Vector<Type, 3> operator * (const Vector<Type, 3>& v) const
      {
        Vector<Type, 3> vErg;

        vErg[0] = m_m[0][0] * v[0] + m_m[1][0] * v[1] + m_m[2][0] * v[2];
        vErg[1] = m_m[0][1] * v[0] + m_m[1][1] * v[1] + m_m[2][1] * v[2];
        vErg[2] = m_m[0][2] * v[0] + m_m[1][2] * v[1] + m_m[2][2] * v[2];

        return vErg;
      }

      inline Matrix& operator += (const Matrix& m)
      {
		    m_m[0] += m[0];
        m_m[1] += m[1];
        m_m[2] += m[2];

        return *this;
      }

      inline Matrix& operator -= (const Matrix& m)
      {
		    m_m[0] -= m[0];
        m_m[1] -= m[1];
        m_m[2] -= m[2];

        return *this;
      }

      inline Matrix& operator *= (const Matrix& m)
      {
        Matrix<Type, 3> tmpMat = *this;

		    m_m[0][0] = tmpMat[0][0] * m[0][0] + tmpMat[1][0] * m[0][1] + tmpMat[2][0] * m[0][2];
        m_m[0][1] = tmpMat[0][1] * m[0][0] + tmpMat[1][1] * m[0][1] + tmpMat[2][1] * m[0][2];
        m_m[0][2] = tmpMat[0][2] * m[0][0] + tmpMat[1][2] * m[0][1] + tmpMat[2][2] * m[0][2];

        m_m[1][0] = tmpMat[0][0] * m[1][0] + tmpMat[1][0] * m[1][1] + tmpMat[2][0] * m[1][2];
        m_m[1][1] = tmpMat[0][1] * m[1][0] + tmpMat[1][1] * m[1][1] + tmpMat[2][1] * m[1][2];
        m_m[1][2] = tmpMat[0][2] * m[1][0] + tmpMat[1][2] * m[1][1] + tmpMat[2][2] * m[1][2];

        m_m[2][0] = tmpMat[0][0] * m[2][0] + tmpMat[1][0] * m[2][1] + tmpMat[2][0] * m[2][2];
        m_m[2][1] = tmpMat[0][1] * m[2][0] + tmpMat[1][1] * m[2][1] + tmpMat[2][1] * m[2][2];
        m_m[2][2] = tmpMat[0][2] * m[2][0] + tmpMat[1][2] * m[2][1] + tmpMat[2][2] * m[2][2];

        return *this;
      }

      inline Matrix& operator *= (Type s)
      {
        m_m[0] *= s;
        m_m[1] *= s;
        m_m[2] *= s;

        return *this;
      }

      private:

      Vector<Type, 3> m_m[3];
    };

    template<typename Type> std::ofstream& operator<<(std::ofstream& stream, const Matrix<Type, 3>& matrix)
    {
      for(unsigned int i = 0; i < 3; i++)
      {
        stream << matrix[i];
      }
      stream << std::endl;

      return stream;
    }

    template<typename Type> std::ifstream& operator>>(std::ifstream& stream, Matrix<Type, 3>& matrix)
    {
      for(unsigned int i = 0; i < 3; i++)
      {
        stream >> matrix[i];
      }

      return stream;
    }
	}
}

#endif

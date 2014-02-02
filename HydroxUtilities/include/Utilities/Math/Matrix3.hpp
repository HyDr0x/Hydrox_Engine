#ifndef MATRIX3_H_
#define MATRIX3_H_

#include "Vector.h"

namespace he
{
	namespace util
	{
    template<typename Type> class Matrix<Type, 3>
    {
      public:

      Matrix()
      {}

      Matrix(const Matrix<Type, 3>& M)
      {
        m[0] = M.m[0];
        m[1] = M.m[1];
        m[2] = M.m[2];
      }


      Matrix(Vector<Type, 3>& v0, Vector<Type, 3>& v1, Vector<Type, 3>& v2)
      {
        m[0][0]=v0[0]; m[1][0]=v1[0]; m[2][0]=v2[0];
        m[0][1]=v0[1]; m[1][1]=v1[1]; m[2][1]=v2[1];
        m[0][2]=v0[2]; m[1][2]=v1[2]; m[2][2]=v2[2];
      }

      Matrix(const Type *a)
      {
        for(int i = 0; i < 3; i++)
        {
          for(int j = 0; j < 3; j++)
          {
            m[i][j] = a[4 * i + j];
          }
        }
      }

      Matrix(Type m_11, Type m_21, Type m_31,
          Type m_12, Type m_22, Type m_32,
          Type m_13, Type m_23, Type m_33)
      {
        m[0][0]=m_11; m[1][0]=m_21; m[2][0]=m_31;
        m[0][1]=m_12; m[1][1]=m_22; m[2][1]=m_32;
        m[0][2]=m_13; m[1][2]=m_23; m[2][2]=m_33;
      }

	    ~Matrix(){}

      static inline Matrix identity()
      {
        return Matrix<Type, 3>(1.0f,  0.0f,  0.0f,
                            0.0f,  1.0f,  0.0f,
                            0.0f,  0.0f,  1.0f);
      }

      inline Vector<Type, 3>& operator [](unsigned int i)
	    {
	      return m[i];
	    }

	    inline  Type detMat(void)
	    {
	      return m[0][0]*(m[1][1]*m[2][2]-m[2][1]*m[1][2])+
                m[0][1]*(m[1][2]*m[2][0]-m[2][2]*m[1][0])+
                m[0][2]*(m[1][0]*m[2][1]-m[1][1]*m[2][0]);
	    }

	    inline  Matrix invert(void)
	    {
		    Type invDetM=detMat();
		    Matrix invM;
		    if(invDetM)
		    {
			    invDetM = 1.0f/invDetM;
			
          invM[0][0] =  m[1][1]*m[2][2]-m[1][2]*m[2][1];
          invM[0][1] =-(m[0][1]*m[2][2]-m[0][2]*m[2][1]);
          invM[0][2] =  m[0][1]*m[1][2]-m[0][2]*m[1][1];
          invM[1][0] =-(m[1][0]*m[2][2]-m[1][2]*m[2][0]); 
          invM[1][1] =  m[0][0]*m[2][2]-m[0][2]*m[2][0]; 
          invM[1][2] =-(m[0][0]*m[1][2]-m[0][2]*m[1][0]);
          invM[2][0] =  m[1][0]*m[2][1]-m[1][1]*m[2][0]; 
          invM[2][1] =-(m[0][0]*m[2][1]-m[0][1]*m[2][0]); 
          invM[2][2] =  m[0][0]*m[1][1]-m[0][1]*m[1][0];

          invM *= invDetM;
		    }
		    return invM;
	    }

      inline  Matrix transpose(void)
      {
        return Matrix<Type, 3>(m[0][0], m[0][1], m[0][2],
                            m[1][0], m[1][1], m[1][2],
                            m[2][0], m[2][1], m[2][2]);
      }

      inline  Vector<Type, 3> operator * ( Vector<Type, 3>& v )//SSE4.1
      {
        Vector<Type, 3> vErg;

        vErg[0] = m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2];
        vErg[1] = m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2];
        vErg[2] = m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2];

        return vErg;
      }


      inline  Matrix operator * (const Type s )
      {
        Matrix erg;

        erg[0] = m[0] * s;
        erg[1] = m[1] * s;
        erg[2] = m[2] * s;

        return erg;
      }

      inline  Matrix& operator *= ( Type s )
      {
        m[0] *= s;
        m[1] *= s;
        m[2] *= s;

        return *this;
      }

      inline  Matrix operator + (const Matrix& M)
      {
        Matrix erg;

        erg[0] = m[0] + M[0];
        erg[1] = m[1] + M[1];
        erg[2] = m[2] + M[2];

        return erg;
      }

      inline  Matrix operator - (const Matrix& M)
      {
		    Matrix erg;

        erg[0] = m[0] - M[0];
        erg[1] = m[1] - M[1];
        erg[2] = m[2] - M[2];

        return erg;
      }

      inline const Matrix& operator += (const Matrix& M)
      {
		    m[0] += M[0];
        m[1] += M[1];
        m[2] += M[2];

        return *this;
      }

      inline const Matrix& operator -= (const Matrix& M)
      {
		    m[0] -= M[0];
        m[1] -= M[1];
        m[2] -= M[2];

        return *this;
      }

      inline Matrix<Type, 3> operator * (Matrix<Type, 3>& M1)
      {
        Matrix<Type, 3> erg;

        erg[0][0] = m[0][0] * M1[0][0] + m[1][0] * M1[0][1] + m[2][0] * M1[0][2];
        erg[0][1] = m[0][1] * M1[0][0] + m[1][1] * M1[0][1] + m[2][1] * M1[0][2];
        erg[0][2] = m[0][2] * M1[0][0] + m[1][2] * M1[0][1] + m[2][2] * M1[0][2];

        erg[1][0] = m[0][0] * M1[1][0] + m[1][0] * M1[1][1] + m[2][0] * M1[1][2];
        erg[1][1] = m[0][1] * M1[1][0] + m[1][1] * M1[1][1] + m[2][1] * M1[1][2];
        erg[1][2] = m[0][2] * M1[1][0] + m[1][2] * M1[1][1] + m[2][2] * M1[1][2];

        erg[2][0] = m[0][0] * M1[2][0] + m[1][0] * M1[2][1] + m[2][0] * M1[2][2];
        erg[2][1] = m[0][1] * M1[2][0] + m[1][1] * M1[2][1] + m[2][1] * M1[2][2];
        erg[2][2] = m[0][2] * M1[2][0] + m[1][2] * M1[2][1] + m[2][2] * M1[2][2];

        return erg;
      }

      inline const Matrix<Type, 3>& operator *= (Matrix<Type, 3>& M1)
      {
        Matrix<Type, 3> M0 = *this;

		    m[0][0] = M0[0][0] * M1[0][0] + M0[1][0] * M1[0][1] + M0[2][0] * M1[0][2];
        m[0][1] = M0[0][1] * M1[0][0] + M0[1][1] * M1[0][1] + M0[2][1] * M1[0][2];
        m[0][2] = M0[0][2] * M1[0][0] + M0[1][2] * M1[0][1] + M0[2][2] * M1[0][2];

        m[1][0] = M0[0][0] * M1[1][0] + M0[1][0] * M1[1][1] + M0[2][0] * M1[1][2];
        m[1][1] = M0[0][1] * M1[1][0] + M0[1][1] * M1[1][1] + M0[2][1] * M1[1][2];
        m[1][2] = M0[0][2] * M1[1][0] + M0[1][2] * M1[1][1] + M0[2][2] * M1[1][2];

        m[2][0] = M0[0][0] * M1[2][0] + M0[1][0] * M1[2][1] + M0[2][0] * M1[2][2];
        m[2][1] = M0[0][1] * M1[2][0] + M0[1][1] * M1[2][1] + M0[2][1] * M1[2][2];
        m[2][2] = M0[0][2] * M1[2][0] + M0[1][2] * M1[2][1] + M0[2][2] * M1[2][2];

        return *this;
      }

      private:

      Vector<Type, 3> m[3];
    };
	}
}

#endif

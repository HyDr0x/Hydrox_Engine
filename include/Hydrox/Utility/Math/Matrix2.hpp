#ifndef MATRIX2_H_
#define MATRIX2_H_

#include "Vector.h"

namespace he
{
  template<typename Type> class Matrix<Type, 2>
  {
    public:

    Matrix()
    {}

    Matrix(const Matrix<Type, 2>& M)
    {
      m[0] = M.m[0];
      m[1] = M.m[1];
    }


    Matrix(Vector<Type, 2>& v0, Vector<Type, 2>& v1)
    {
      m[0][0]=v0[0];  m[1][0]=v0[1];
      m[0][1]=v1[0];  m[1][1]=v1[1];
    }

    Matrix(const Type *a)
    {
      for(int i = 0; i < 2; i++)
      {
        for(int j = 0; j < 2; j++)
        {
          m[i][j] = a[4 * i + j];
        }
      }
    }

    Matrix(Type m_11,  Type m_21,
        Type m_12,  Type m_22)
    {
      m[0][0]=m_11;  m[1][0]=m_21;
      m[0][1]=m_12;  m[1][1]=m_22;
    }

	  ~Matrix(){}

    static inline Matrix identity()
    {
      return Matrix<Type, 2>(1.0f,  0.0f,
                          0.0f,  1.0f);
    }

    inline Vector<Type, 2>& operator [](unsigned int i)
	  {
	    return m[i];
	  }

	  inline Type detMat(void)
	  {
	    return m[0][0] * m[1][1] - m[1][0] * m[0][1];
	  }

	  inline Matrix invert(void)
	  {
		  Type invDetM=detMat();
      Matrix invM;

		  if(invDetM)
		  {
			  invDetM=1.0f/invDetM;

        invM[0][0] = m[1][1];
        invM[1][0] =-m[0][1];
        invM[0][1] =-m[1][0];
        invM[1][1] = m[0][0];

        invM *= invDetM;

        return invM;
		  }

		  return invM;
	  }

    inline  Matrix transpose(void)
    {
      return Matrix<Type, 2>(m[0][0], m[0][1],
                          m[1][0], m[1][1]);
    }

    inline Vector<Type, 2> operator * ( Vector<Type,2>& v )
    {
      Vector<Type, 2> vErg;

      vErg[0] = m[0][0] * v[0] + m[1][0] * v[1];
      vErg[1] = m[0][1] * v[0] + m[1][1] * v[1];

      return vErg;
    }

    inline  Matrix operator * ( Type s )
    {
      Matrix erg;

      erg[0] = m[0] * s;
      erg[1] = m[1] * s;

      return erg;
    }

    inline const Matrix& operator *= ( Type s )
    {
      m[0] *= s;
      m[1] *= s;

      return *this;
    }

    inline Matrix operator + (const Matrix& M)
    {
      Matrix erg;

      erg[0] = m[0] + M[0];
      erg[1] = m[1] + M[1];

      return erg;
    }

    inline Matrix operator - (const Matrix& M)
    {
		  Matrix erg;

      erg[0] = m[0] - M[0];
      erg[1] = m[1] - M[1];

      return erg;
    }

    inline const Matrix& operator += (const Matrix& M)
    {
		  m[0] += M[0];
      m[1] += M[1];

      return *this;
    }

    inline  Matrix& operator -= ( Matrix& M)
    {
		  m[0] -= M[0];
      m[1] -= M[1];

      return *this;
    }


    inline Matrix<Type,2> operator * (Matrix<Type, 2>& M1)
    {
      Matrix<Type, 2> erg;

      erg[0][0] = m[0][0] * M1[0][0] + m[1][0] * M1[0][1];
      erg[0][1] = m[0][1] * M1[0][0] + m[1][1] * M1[0][1];

      erg[1][0] = m[0][0] * M1[1][0] + m[1][0] * M1[1][1];
      erg[1][1] = m[0][1] * M1[1][0] + m[1][1] * M1[1][1];

      return erg;
    }

    inline const Matrix<Type,2>& operator *= ( Matrix<Type, 2>& M1)
    {
      Matrix<Type, 2> M0 = *this;

		  m[0][0] = M0[0][0] * M1[0][0] + M0[1][0] * M1[0][1];
      m[0][1] = M0[0][1] * M1[0][0] + M0[1][1] * M1[0][1];

      m[1][0] = M0[0][0] * M1[1][0] + M0[1][0] * M1[1][1];
      m[1][1] = M0[0][1] * M1[1][0] + M0[1][1] * M1[1][1];

      return *this;
    }

  private:

    Vector<Type, 2> m[2];
  };
}

#endif

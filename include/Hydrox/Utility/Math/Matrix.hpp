#ifndef MATRIX_H_
#define MATRIX_H_

#include "Vector.h"

namespace he
{
  template< typename Type, unsigned int Dim> class Matrix;

  template< typename Type> class Matrix<Type, 4>
  {
    public:

    Matrix()
    {}

    Matrix(const Matrix<Type, 4>& M)
    {
      m[0] = M.m[0];
      m[1] = M.m[1];
      m[2] = M.m[2];
      m[3] = M.m[3];
    }


    Matrix(const Vector<Type, 4>& v0, const Vector<Type, 4>& v1, const Vector<Type, 4>& v2, const Vector<Type, 4>& v3 )
    {
      m[0][0]=v0[0];  m[1][0]=v1[0];  m[2][0]=v2[0];  m[3][0]=v3[0];
      m[0][1]=v0[1];  m[1][1]=v1[1];  m[2][1]=v2[1];  m[3][1]=v3[1];
      m[0][2]=v0[2];  m[1][2]=v1[2];  m[2][2]=v2[2];  m[3][2]=v3[2];
      m[0][3]=v0[3];  m[1][3]=v1[3];  m[2][3]=v2[3];  m[3][3]=v3[3];
    }

    Matrix(const Type *a)
    {
      for(int i = 0; i < 4; i++)
      {
        for(int j = 0; j < 4; j++)
        {
          m[i][j] = a[4 * i + j];
        }
      }
    }

    Matrix(  Type m_11,  Type m_21,  Type m_31,  Type m_41,
          Type m_12,  Type m_22,  Type m_32,  Type m_42,
          Type m_13,  Type m_23,  Type m_33,  Type m_43,
          Type m_14,  Type m_24,  Type m_34,  Type m_44)
    {
      m[0][0]=m_11;  m[1][0]=m_21;  m[2][0]=m_31;  m[3][0]=m_41;
      m[0][1]=m_12;  m[1][1]=m_22;  m[2][1]=m_32;  m[3][1]=m_42;
      m[0][2]=m_13;  m[1][2]=m_23;  m[2][2]=m_33;  m[3][2]=m_43;
      m[0][3]=m_14;  m[1][3]=m_24;  m[2][3]=m_34;  m[3][3]=m_44;
    }

	  ~Matrix(){}

    static inline Matrix identity()
    {
      return Matrix<Type, 4>(1.0f,  0.0f,  0.0f,  0.0f,
                          0.0f,  1.0f,  0.0f,  0.0f,
                          0.0f,  0.0f,  1.0f,  0.0f,
                          0.0f,  0.0f,  0.0f,  1.0f);
    }

    inline Vector<Type, 4>& operator [](unsigned int i)
	  {
	    return m[i];
	  }

	  inline  Type detMat(void)
	  {
	    return m[0][0] * (m[1][1]*m[2][2]*m[3][3] + m[2][1]*m[3][2]*m[1][3] + m[3][1]*m[1][2]*m[2][3] - m[1][1]*m[3][2]*m[2][3] - m[2][1]*m[1][2]*m[3][3] - m[3][1]*m[2][2]*m[1][3])+
              m[1][0] * (m[0][1]*m[3][2]*m[2][3] + m[2][1]*m[0][2]*m[3][3] + m[3][1]*m[2][2]*m[0][3] - m[0][1]*m[2][2]*m[3][3] - m[2][1]*m[3][2]*m[0][3] - m[3][1]*m[0][2]*m[2][3])+
              m[2][0] * (m[0][1]*m[1][2]*m[3][3] + m[1][1]*m[3][2]*m[0][3] + m[3][1]*m[0][2]*m[1][3] - m[0][1]*m[3][2]*m[1][3] - m[1][1]*m[0][2]*m[3][3] - m[3][1]*m[1][2]*m[0][3])+
              m[3][0] * (m[0][1]*m[2][2]*m[1][3] + m[1][1]*m[0][2]*m[2][3] + m[2][1]*m[1][2]*m[0][3] - m[0][1]*m[1][2]*m[2][3] - m[1][1]*m[2][2]*m[0][3] - m[2][1]*m[0][2]*m[1][3]);
	  }

	  inline  Matrix invert(void)
	  {
		  Type InvDetM = detMat();
      Matrix invMat;

		  if(InvDetM)
		  {
			  InvDetM = 1.0f / InvDetM;

        invMat[0][0] = m[1][1]*m[2][2]*m[3][3] + m[2][1]*m[3][2]*m[1][3] + m[3][1]*m[1][2]*m[2][3] - m[1][1]*m[3][2]*m[2][3] - m[2][1]*m[1][2]*m[3][3] - m[3][1]*m[2][2]*m[1][3];
        invMat[1][0] = m[1][0]*m[3][2]*m[2][3] + m[2][0]*m[1][2]*m[3][3] + m[3][0]*m[2][2]*m[1][3] - m[1][0]*m[2][2]*m[3][3] - m[2][0]*m[3][2]*m[1][3] - m[3][0]*m[1][2]*m[2][3];
        invMat[2][0] = m[1][0]*m[2][1]*m[3][3] + m[2][0]*m[3][1]*m[1][3] + m[3][0]*m[1][1]*m[2][3] - m[1][0]*m[3][1]*m[2][3] - m[2][0]*m[1][1]*m[3][3] - m[3][0]*m[2][1]*m[1][3];
        invMat[3][0] = m[1][0]*m[3][1]*m[2][2] + m[2][0]*m[1][1]*m[3][2] + m[3][0]*m[2][1]*m[1][2] - m[1][0]*m[2][1]*m[3][2] - m[2][0]*m[3][1]*m[1][2] - m[3][0]*m[1][1]*m[2][2];

        invMat[0][1] = m[0][1]*m[3][2]*m[2][3] + m[2][1]*m[0][2]*m[3][3] + m[3][1]*m[2][2]*m[0][3] - m[0][1]*m[2][2]*m[3][3] - m[2][1]*m[3][2]*m[0][3] - m[3][1]*m[0][2]*m[2][3];
        invMat[1][1] = m[0][0]*m[2][2]*m[3][3] + m[2][0]*m[3][2]*m[0][3] + m[3][0]*m[0][2]*m[2][3] - m[0][0]*m[3][2]*m[2][3] - m[2][0]*m[0][2]*m[3][3] - m[3][0]*m[2][2]*m[0][3];
        invMat[2][1] = m[0][0]*m[3][1]*m[2][3] + m[2][0]*m[0][1]*m[3][3] + m[3][0]*m[2][1]*m[0][3] - m[0][0]*m[2][1]*m[3][3] - m[2][0]*m[3][1]*m[0][3] - m[3][0]*m[0][1]*m[2][3];
        invMat[3][1] = m[0][0]*m[2][1]*m[3][2] + m[2][0]*m[3][1]*m[0][2] + m[3][0]*m[0][1]*m[2][2] - m[0][0]*m[3][1]*m[2][2] - m[2][0]*m[0][1]*m[3][2] - m[3][0]*m[2][1]*m[0][2];

        invMat[0][2] = m[0][1]*m[1][2]*m[3][3] + m[1][1]*m[3][2]*m[0][3] + m[3][1]*m[0][2]*m[1][3] - m[0][1]*m[3][2]*m[1][3] - m[1][1]*m[0][2]*m[3][3] - m[3][1]*m[1][2]*m[0][3];
        invMat[1][2] = m[0][0]*m[3][2]*m[1][3] + m[1][0]*m[0][2]*m[3][3] + m[3][0]*m[1][2]*m[0][3] - m[0][0]*m[1][2]*m[3][3] - m[1][0]*m[3][2]*m[0][3] - m[3][0]*m[0][2]*m[1][3];
        invMat[2][2] = m[0][0]*m[1][1]*m[3][3] + m[1][0]*m[3][1]*m[0][3] + m[3][0]*m[0][1]*m[1][3] - m[0][0]*m[3][1]*m[1][3] - m[1][0]*m[0][1]*m[3][3] - m[3][0]*m[1][1]*m[0][3];
        invMat[3][2] = m[0][0]*m[3][1]*m[1][2] + m[1][0]*m[0][1]*m[3][2] + m[3][0]*m[1][1]*m[0][2] - m[0][0]*m[1][1]*m[3][2] - m[1][0]*m[3][1]*m[0][2] - m[3][0]*m[0][1]*m[1][2];

        invMat[0][3] = m[0][1]*m[2][2]*m[1][3] + m[1][1]*m[0][2]*m[2][3] + m[2][1]*m[1][2]*m[0][3] - m[0][1]*m[1][2]*m[2][3] - m[1][1]*m[2][2]*m[0][3] - m[2][1]*m[0][2]*m[1][3];
        invMat[1][3] = m[0][0]*m[1][2]*m[2][3] + m[1][0]*m[2][2]*m[0][3] + m[2][0]*m[0][2]*m[1][3] - m[0][0]*m[2][2]*m[1][3] - m[1][0]*m[0][2]*m[2][3] - m[2][0]*m[1][2]*m[0][3];
        invMat[2][3] = m[0][0]*m[2][1]*m[1][3] + m[1][0]*m[0][1]*m[2][3] + m[2][0]*m[1][1]*m[0][3] - m[0][0]*m[1][1]*m[2][3] - m[1][0]*m[2][1]*m[0][3] - m[2][0]*m[0][1]*m[1][3];
        invMat[3][3] = m[0][0]*m[1][1]*m[2][2] + m[1][0]*m[2][1]*m[0][2] + m[2][0]*m[0][1]*m[1][2] - m[0][0]*m[2][1]*m[1][2] - m[1][0]*m[0][1]*m[2][2] - m[2][0]*m[1][1]*m[0][2];

        invMat *= InvDetM;
		  }

      return invMat;
    }

    inline Matrix transpose(void)
    {
      return Matrix<Type, 4>(m[0][0], m[0][1], m[0][2], m[0][3],
                          m[1][0], m[1][1], m[1][2], m[1][3],
                          m[2][0], m[2][1], m[2][2], m[2][3],
                          m[3][0], m[3][1], m[3][2], m[3][3]);
    }

    inline Vector<Type, 4> operator * ( Vector<Type, 4>& v )//SSE4.1
    {
      Vector<Type, 4> vErg;

      vErg[0] = m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2] + m[3][0] * v[3];
      vErg[1] = m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2] + m[3][1] * v[3];
      vErg[2] = m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2] + m[3][2] * v[3];
      vErg[3] = m[0][3] * v[0] + m[1][3] * v[1] + m[2][3] * v[2] + m[3][3] * v[3];

      return vErg;
    }

    inline Matrix operator * ( Type s )
    {
      Matrix erg;

      erg[0] = m[0] * s;
      erg[1] = m[1] * s;
      erg[2] = m[2] * s;
      erg[3] = m[3] * s;

      return erg;
    }

    inline const Matrix& operator *= ( Type s )
    {
      m[0] *= s;
      m[1] *= s;
      m[2] *= s;
      m[3] *= s;

      return *this;
    }

    inline Matrix operator + (const Matrix& M)
    {
      Matrix erg;

      erg[0] = m[0] + M[0];
      erg[1] = m[1] + M[1];
      erg[2] = m[2] + M[2];
      erg[3] = m[3] + M[3];

      return erg;
    }

    inline  Matrix operator - ( Matrix& M)
    {
		  Matrix erg;

      erg[0] = m[0] - M[0];
      erg[1] = m[1] - M[1];
      erg[2] = m[2] - M[2];
      erg[3] = m[3] - M[3];

      return erg;
    }

    inline Matrix& operator += ( Matrix& M)
    {
		  m[0] += M[0];
      m[1] += M[1];
      m[2] += M[2];
      m[3] += M[3];

      return *this;
    }

    inline Matrix& operator -= ( Matrix& M)
    {
		  m[0] -= M[0];
      m[1] -= M[1];
      m[2] -= M[2];
      m[3] -= M[3];

      return *this;
    }

    inline Matrix<Type, 4> operator * (Matrix<Type, 4>& M1)
    {
      Matrix<Type, 4> erg;

      erg[0][0] = m[0][0] * M1[0][0] + m[1][0] * M1[0][1] + m[2][0] * M1[0][2] + m[3][0] * M1[0][3];
      erg[0][1] = m[0][1] * M1[0][0] + m[1][1] * M1[0][1] + m[2][1] * M1[0][2] + m[3][1] * M1[0][3];
      erg[0][2] = m[0][2] * M1[0][0] + m[1][2] * M1[0][1] + m[2][2] * M1[0][2] + m[3][2] * M1[0][3];
      erg[0][3] = m[0][3] * M1[0][0] + m[1][3] * M1[0][1] + m[2][3] * M1[0][2] + m[3][3] * M1[0][3];

      erg[1][0] = m[0][0] * M1[1][0] + m[1][0] * M1[1][1] + m[2][0] * M1[1][2] + m[3][0] * M1[1][3];
      erg[1][1] = m[0][1] * M1[1][0] + m[1][1] * M1[1][1] + m[2][1] * M1[1][2] + m[3][1] * M1[1][3];
      erg[1][2] = m[0][2] * M1[1][0] + m[1][2] * M1[1][1] + m[2][2] * M1[1][2] + m[3][2] * M1[1][3];
      erg[1][3] = m[0][3] * M1[1][0] + m[1][3] * M1[1][1] + m[2][3] * M1[1][2] + m[3][3] * M1[1][3];

      erg[2][0] = m[0][0] * M1[2][0] + m[1][0] * M1[2][1] + m[2][0] * M1[2][2] + m[3][0] * M1[2][3];
      erg[2][1] = m[0][1] * M1[2][0] + m[1][1] * M1[2][1] + m[2][1] * M1[2][2] + m[3][1] * M1[2][3];
      erg[2][2] = m[0][2] * M1[2][0] + m[1][2] * M1[2][1] + m[2][2] * M1[2][2] + m[3][2] * M1[2][3];
      erg[2][3] = m[0][3] * M1[2][0] + m[1][3] * M1[2][1] + m[2][3] * M1[2][2] + m[3][3] * M1[2][3];

      erg[3][0] = m[0][0] * M1[3][0] + m[1][0] * M1[3][1] + m[2][0] * M1[3][2] + m[3][0] * M1[3][3];
      erg[3][1] = m[0][1] * M1[3][0] + m[1][1] * M1[3][1] + m[2][1] * M1[3][2] + m[3][1] * M1[3][3];
      erg[3][2] = m[0][2] * M1[3][0] + m[1][2] * M1[3][1] + m[2][2] * M1[3][2] + m[3][2] * M1[3][3];
      erg[3][3] = m[0][3] * M1[3][0] + m[1][3] * M1[3][1] + m[2][3] * M1[3][2] + m[3][3] * M1[3][3];

      return erg;
    }

    inline Matrix<Type, 4>& operator *= (Matrix<Type, 4>& M1)
    {
      Matrix<Type, 4> M0 = *this;

		  m[0][0] = M0[0][0] * M1[0][0] + M0[1][0] * M1[0][1] + M0[2][0] * M1[0][2] + M0[3][0] * M1[0][3];
      m[0][1] = M0[0][1] * M1[0][0] + M0[1][1] * M1[0][1] + M0[2][1] * M1[0][2] + M0[3][1] * M1[0][3];
      m[0][2] = M0[0][2] * M1[0][0] + M0[1][2] * M1[0][1] + M0[2][2] * M1[0][2] + M0[3][2] * M1[0][3];
      m[0][3] = M0[0][3] * M1[0][0] + M0[1][3] * M1[0][1] + M0[2][3] * M1[0][2] + M0[3][3] * M1[0][3];

      m[1][0] = M0[0][0] * M1[1][0] + M0[1][0] * M1[1][1] + M0[2][0] * M1[1][2] + M0[3][0] * M1[1][3];
      m[1][1] = M0[0][1] * M1[1][0] + M0[1][1] * M1[1][1] + M0[2][1] * M1[1][2] + M0[3][1] * M1[1][3];
      m[1][2] = M0[0][2] * M1[1][0] + M0[1][2] * M1[1][1] + M0[2][2] * M1[1][2] + M0[3][2] * M1[1][3];
      m[1][3] = M0[0][3] * M1[1][0] + M0[1][3] * M1[1][1] + M0[2][3] * M1[1][2] + M0[3][3] * M1[1][3];

      m[2][0] = M0[0][0] * M1[2][0] + M0[1][0] * M1[2][1] + M0[2][0] * M1[2][2] + M0[3][0] * M1[2][3];
      m[2][1] = M0[0][1] * M1[2][0] + M0[1][1] * M1[2][1] + M0[2][1] * M1[2][2] + M0[3][1] * M1[2][3];
      m[2][2] = M0[0][2] * M1[2][0] + M0[1][2] * M1[2][1] + M0[2][2] * M1[2][2] + M0[3][2] * M1[2][3];
      m[2][3] = M0[0][3] * M1[2][0] + M0[1][3] * M1[2][1] + M0[2][3] * M1[2][2] + M0[3][3] * M1[2][3];

      m[3][0] = M0[0][0] * M1[3][0] + M0[1][0] * M1[3][1] + M0[2][0] * M1[3][2] + M0[3][0] * M1[3][3];
      m[3][1] = M0[0][1] * M1[3][0] + M0[1][1] * M1[3][1] + M0[2][1] * M1[3][2] + M0[3][1] * M1[3][3];
      m[3][2] = M0[0][2] * M1[3][0] + M0[1][2] * M1[3][1] + M0[2][2] * M1[3][2] + M0[3][2] * M1[3][3];
      m[3][3] = M0[0][3] * M1[3][0] + M0[1][3] * M1[3][1] + M0[2][3] * M1[3][2] + M0[3][3] * M1[3][3];

      return *this;
    }

    private:

    Vector<Type, 4> m[4];
  };




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
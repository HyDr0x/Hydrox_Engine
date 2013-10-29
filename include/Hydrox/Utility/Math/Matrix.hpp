#ifndef MATRIX_H_
#define MATRIX_H_

#include "Vector.h"

//#define SSE
//#define SSE2
#define SSE4

#if defined SSE
#   include <xmmintrin.h> /*SSE*/
#endif

#if defined SSE2
#   include <xmmintrin.h> /*SSE*/
#   include <emmintrin.h> /*SSE2*/
#endif

#if defined SSE4
#   include <xmmintrin.h> /*SSE*/
#   include <emmintrin.h> /*SSE2*/
#   include <smmintrin.h> /* SSE4.1 */
#   include <nmmintrin.h> /* SSE4.2 */
#endif



template< typename Type, unsigned int Dim> class Mat;

template< typename Type> class Mat<Type,4>
{
  public:

  Mat()
  {}

  Mat(const Mat<Type,4>& M)
  {
    m[0] = M.m[0];
    m[1] = M.m[1];
    m[2] = M.m[2];
    m[3] = M.m[3];
  }


  Mat(const Vec<Type,4>& v0, const Vec<Type,4>& v1, const Vec<Type,4>& v2, const Vec<Type,4>& v3 )
  {
    m[0][0]=v0[0];  m[1][0]=v1[0];  m[2][0]=v2[0];  m[3][0]=v3[0];
    m[0][1]=v0[1];  m[1][1]=v1[1];  m[2][1]=v2[1];  m[3][1]=v3[1];
    m[0][2]=v0[2];  m[1][2]=v1[2];  m[2][2]=v2[2];  m[3][2]=v3[2];
    m[0][3]=v0[3];  m[1][3]=v1[3];  m[2][3]=v2[3];  m[3][3]=v3[3];
  }

  Mat(const Type *a)
  {
    for(int i = 0; i < 4; i++)
    {
      for(int j = 0; j < 4; j++)
      {
        m[i][j] = a[4 * i + j];
      }
    }
  }

  Mat( Type m_11,  Type m_21,  Type m_31,  Type m_41,
        Type m_12,  Type m_22,  Type m_32,  Type m_42,
        Type m_13,  Type m_23,  Type m_33,  Type m_43,
        Type m_14,  Type m_24,  Type m_34,  Type m_44)
  {
    m[0][0]=m_11;  m[1][0]=m_21;  m[2][0]=m_31;  m[3][0]=m_41;
    m[0][1]=m_12;  m[1][1]=m_22;  m[2][1]=m_32;  m[3][1]=m_42;
    m[0][2]=m_13;  m[1][2]=m_23;  m[2][2]=m_33;  m[3][2]=m_43;
    m[0][3]=m_14;  m[1][3]=m_24;  m[2][3]=m_34;  m[3][3]=m_44;
  }

	~Mat(){}

  static __forceinline Mat identity()
  {
    return Mat<Type,4>(1.0f,  0.0f,  0.0f,  0.0f,
                        0.0f,  1.0f,  0.0f,  0.0f,
                        0.0f,  0.0f,  1.0f,  0.0f,
                        0.0f,  0.0f,  0.0f,  1.0f);
  }

  __forceinline Vec<Type,4>& operator [](unsigned int i)
	{
	  return m[i];
	}

	__forceinline  Type detMat(void)
	{
	  return m[0][0] * (m[1][1]*m[2][2]*m[3][3] + m[2][1]*m[3][2]*m[1][3] + m[3][1]*m[1][2]*m[2][3] - m[1][1]*m[3][2]*m[2][3] - m[2][1]*m[1][2]*m[3][3] - m[3][1]*m[2][2]*m[1][3])+
            m[1][0] * (m[0][1]*m[3][2]*m[2][3] + m[2][1]*m[0][2]*m[3][3] + m[3][1]*m[2][2]*m[0][3] - m[0][1]*m[2][2]*m[3][3] - m[2][1]*m[3][2]*m[0][3] - m[3][1]*m[0][2]*m[2][3])+
            m[2][0] * (m[0][1]*m[1][2]*m[3][3] + m[1][1]*m[3][2]*m[0][3] + m[3][1]*m[0][2]*m[1][3] - m[0][1]*m[3][2]*m[1][3] - m[1][1]*m[0][2]*m[3][3] - m[3][1]*m[1][2]*m[0][3])+
            m[3][0] * (m[0][1]*m[2][2]*m[1][3] + m[1][1]*m[0][2]*m[2][3] + m[2][1]*m[1][2]*m[0][3] - m[0][1]*m[1][2]*m[2][3] - m[1][1]*m[2][2]*m[0][3] - m[2][1]*m[0][2]*m[1][3]);
	}

	__forceinline  Mat invert(void)
	{
		Type InvDetM = detMat();
    Mat invMat;

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

  __forceinline Mat transpose(void)
  {
    return Mat<Type,4>(m[0][0], m[0][1], m[0][2], m[0][3],
                        m[1][0], m[1][1], m[1][2], m[1][3],
                        m[2][0], m[2][1], m[2][2], m[2][3],
                        m[3][0], m[3][1], m[3][2], m[3][3]);
  }

#if defined(SSE4)
  __forceinline Vec<Type,4> operator * ( Vec<Type,4>& v )//SSE4.1
  {
    __m128 a,b,c;
    Vec<Type,4> vTmp, vErg;
    b=_mm_loadu_ps(&v[0]);

    a=_mm_setr_ps(m[0][0], m[1][0], m[2][0], m[3][0]);
    c=_mm_dp_ps(a,b,0xF1);
    _mm_storeu_ps(&vErg[0],c);

    a=_mm_setr_ps(m[0][1], m[1][1], m[2][1], m[3][1]);
    c=_mm_dp_ps(a,b,0xF2);
    _mm_storeu_ps(&vTmp[0],c);
    vErg[1] = vTmp[1];

    a=_mm_setr_ps(m[0][2], m[1][2], m[2][2], m[3][2]);
    c=_mm_dp_ps(a,b,0xF4);
    _mm_storeu_ps(&vTmp[0],c);
    vErg[2] = vTmp[2];

    a=_mm_setr_ps(m[0][3], m[1][3], m[2][3], m[3][3]);
    c=_mm_dp_ps(a,b,0xF8);
    _mm_storeu_ps(&vTmp[0],c);
    vErg[3] = vTmp[3];

    return vErg;
  }
#endif

  __forceinline Mat operator * ( Type s )
  {
    __m128 a,b,c;
    Mat erg;
    Vec<Type,4> v(s, s, s, s);
    b=_mm_loadu_ps(&v[0]);

    a=_mm_loadu_ps(&(m[0][0]));
    c=_mm_mul_ps(a,b);
    _mm_storeu_ps(&(erg[0][0]),c);

    a=_mm_loadu_ps(&(m[1][0]));
    c=_mm_mul_ps(a,b);
    _mm_storeu_ps(&(erg[1][0]),c);

    a=_mm_loadu_ps(&(m[2][0]));
    c=_mm_mul_ps(a,b);
    _mm_storeu_ps(&(erg[2][0]),c);

    a=_mm_loadu_ps(&(m[3][0]));
    c=_mm_mul_ps(a,b);
    _mm_storeu_ps(&(erg[3][0]),c);

    return erg;
  }

    __forceinline const Mat& operator *= ( Type s )
  {
    __m128 a,b,c;
    Vec<Type,4> v(s, s, s, s);
    b=_mm_loadu_ps(&v[0]);

    a=_mm_loadu_ps(&(m[0][0]));
    c=_mm_mul_ps(a,b);
    _mm_storeu_ps(&(m[0][0]),c);

    a=_mm_loadu_ps(&(m[1][0]));
    c=_mm_mul_ps(a,b);
    _mm_storeu_ps(&(m[1][0]),c);

    a=_mm_loadu_ps(&(m[2][0]));
    c=_mm_mul_ps(a,b);
    _mm_storeu_ps(&(m[2][0]),c);

    a=_mm_loadu_ps(&(m[3][0]));
    c=_mm_mul_ps(a,b);
    _mm_storeu_ps(&(m[3][0]),c);

    return *this;
  }

  __forceinline Mat operator + (const Mat& M)
  {
		__m128 a,b,c;
    Mat erg;
    a=_mm_loadu_ps(&(m[0][0]));
    b=_mm_loadu_ps(M[0]);
    c=_mm_add_ps(a,b);
    _mm_storeu_ps(&(erg[0][0]),c);

    a=_mm_loadu_ps(&(m[1][0]));
    b=_mm_loadu_ps(M[1]);
    c=_mm_add_ps(a,b);
    _mm_storeu_ps(&(erg[1][0]),c);

    a=_mm_loadu_ps(&(m[2][0]));
    b=_mm_loadu_ps(M[2]);
    c=_mm_add_ps(a,b);
    _mm_storeu_ps(&(erg[2][0]),c);

    a=_mm_loadu_ps(&(m[3][0]));
    b=_mm_loadu_ps(M[3]);
    c=_mm_add_ps(a,b);
    _mm_storeu_ps(&(erg[3][0]),c);

    return erg;
  }

  __forceinline  Mat operator - ( Mat& M)
  {
		__m128 a,b,c;
    Mat erg;
    a=_mm_loadu_ps(&(m[0][0]));
    b=_mm_loadu_ps(M[0]);
    c=_mm_sub_ps(a,b);
    _mm_storeu_ps(&(erg[0][0]),c);

    a=_mm_loadu_ps(&(m[1][0]));
    b=_mm_loadu_ps(M[1]);
    c=_mm_sub_ps(a,b);
    _mm_storeu_ps(&(erg[1][0]),c);

    a=_mm_loadu_ps(&(m[2][0]));
    b=_mm_loadu_ps(M[2]);
    c=_mm_sub_ps(a,b);
    _mm_storeu_ps(&(erg[2][0]),c);

    a=_mm_loadu_ps(&(m[3][0]));
    b=_mm_loadu_ps(M[3]);
    c=_mm_sub_ps(a,b);
    _mm_storeu_ps(&(erg[3][0]),c);

    return erg;
  }

    __forceinline Mat& operator += ( Mat& M)
  {
		__m128 a,b,c;
    a=_mm_loadu_ps(&(m[0][0]));
    b=_mm_loadu_ps(M[0]);
    c=_mm_add_ps(a,b);
    _mm_storeu_ps(&(m[0][0]),c);

    a=_mm_loadu_ps(&(m[1][0]));
    b=_mm_loadu_ps(M[1]);
    c=_mm_add_ps(a,b);
    _mm_storeu_ps(&(m[1][0]),c);

    a=_mm_loadu_ps(&(m[2][0]));
    b=_mm_loadu_ps(M[2]);
    c=_mm_add_ps(a,b);
    _mm_storeu_ps(&(m[2][0]),c);

    a=_mm_loadu_ps(&(m[3][0]));
    b=_mm_loadu_ps(M[3]);
    c=_mm_add_ps(a,b);
    _mm_storeu_ps(&(m[3][0]),c);

    return *this;
  }

  __forceinline Mat& operator -= ( Mat& M)
  {
		__m128 a,b,c;
    a=_mm_loadu_ps(&(m[0][0]));
    b=_mm_loadu_ps(M[0]);
    c=_mm_sub_ps(a,b);
    _mm_storeu_ps(&(m[0][0]),c);

    a=_mm_loadu_ps(&(m[1][0]));
    b=_mm_loadu_ps(M[1]);
    c=_mm_sub_ps(a,b);
    _mm_storeu_ps(&(m[1][0]),c);

    a=_mm_loadu_ps(&(m[2][0]));
    b=_mm_loadu_ps(M[2]);
    c=_mm_sub_ps(a,b);
    _mm_storeu_ps(&(m[2][0]),c);

    a=_mm_loadu_ps(&(m[3][0]));
    b=_mm_loadu_ps(M[3]);
    c=_mm_sub_ps(a,b);
    _mm_storeu_ps(&(m[3][0]),c);

    return *this;
  }

#if defined(SSE4)
  friend __forceinline Mat<Type,4> operator * (Mat<Type,4>& M0, Mat<Type,4>& M1) //SSE4, 16 dot protucts vs 64 mult. and 32 add
  {
		__m128 a,b,c,d,e,f;
    Mat<Type,4> m;

		a=_mm_loadu_ps(&(M1[0][0]));
    b=_mm_loadu_ps(&(M1[1][0]));
    c=_mm_loadu_ps(&(M1[2][0]));
    d=_mm_loadu_ps(&(M1[3][0]));

    e = _mm_setr_ps(M0[0][0], M0[1][0], M0[2][0], M0[3][0]);
    f = _mm_dp_ps(a,e,0xF1);
    _mm_store_ss(&m[0][0],f);

    f = _mm_dp_ps(b,e,0xF1);
    _mm_store_ss(&m[1][0],f);

    f = _mm_dp_ps(c,e,0xF1);
    _mm_store_ss(&m[2][0],f);

    f = _mm_dp_ps(d,e,0xF1);
    _mm_store_ss(&m[3][0],f);

    e = _mm_setr_ps(M0[0][1], M0[1][1], M0[2][1], M0[3][1]);
    f = _mm_dp_ps(a,e,0xF1);
    _mm_store_ss(&m[0][1],f);

    f = _mm_dp_ps(b,e,0xF1);
    _mm_store_ss(&m[1][1],f);

    f = _mm_dp_ps(c,e,0xF1);
    _mm_store_ss(&m[2][1],f);

    f = _mm_dp_ps(d,e,0xF1);
    _mm_store_ss(&m[3][1],f);

    e = _mm_setr_ps(M0[0][2], M0[1][2], M0[2][2], M0[3][2]);
    f = _mm_dp_ps(a,e,0xF1);
    _mm_store_ss(&m[0][2],f);

    f = _mm_dp_ps(b,e,0xF1);
    _mm_store_ss(&m[1][2],f);

    f = _mm_dp_ps(c,e,0xF1);
    _mm_store_ss(&m[2][2],f);

    f = _mm_dp_ps(d,e,0xF1);
    _mm_store_ss(&m[3][2],f);

    e = _mm_setr_ps(M0[0][3], M0[1][3], M0[2][3], M0[3][3]);
    f = _mm_dp_ps(a,e,0xF1);
    _mm_store_ss(&m[0][3],f);

    f = _mm_dp_ps(b,e,0xF1);
    _mm_store_ss(&m[1][3],f);

    f = _mm_dp_ps(c,e,0xF1);
    _mm_store_ss(&m[2][3],f);

    f = _mm_dp_ps(d,e,0xF1);
    _mm_store_ss(&m[3][3],f);

    return m;
  }

  __forceinline  Mat<Type,4>& operator *= (Mat<Type,4>& M0) //SSE4, 16 dot protucts vs 64 mult. and 32 add
  {
		__m128 a,b,c,d,e,f;

		a=_mm_loadu_ps(&(M0[0][0]));
    b=_mm_loadu_ps(&(M0[1][0]));
    c=_mm_loadu_ps(&(M0[2][0]));
    d=_mm_loadu_ps(&(M0[3][0]));

    e = _mm_setr_ps(m[0][0], m[1][0], m[2][0], m[3][0]);
    f = _mm_dp_ps(a,e,0xF1);
    _mm_store_ss(&m[0][0],f);

    f = _mm_dp_ps(b,e,0xF1);
    _mm_store_ss(&m[1][0],f);

    f = _mm_dp_ps(c,e,0xF1);
    _mm_store_ss(&m[2][0],f);

    f = _mm_dp_ps(d,e,0xF1);
    _mm_store_ss(&m[3][0],f);

    e = _mm_setr_ps(m[0][1], m[1][1], m[2][1], m[3][1]);
    f = _mm_dp_ps(a,e,0xF1);
    _mm_store_ss(&m[0][1],f);

    f = _mm_dp_ps(b,e,0xF1);
    _mm_store_ss(&m[1][1],f);

    f = _mm_dp_ps(c,e,0xF1);
    _mm_store_ss(&m[2][1],f);

      f = _mm_dp_ps(d,e,0xF1);
    _mm_store_ss(&m[3][1],f);

    e = _mm_setr_ps(m[0][2], m[1][2], m[2][2], m[3][2]);
    f = _mm_dp_ps(a,e,0xF1);
    _mm_store_ss(&m[0][2],f);

    f = _mm_dp_ps(b,e,0xF1);
    _mm_store_ss(&m[1][2],f);

      f = _mm_dp_ps(c,e,0xF1);
    _mm_store_ss(&m[2][2],f);

    f = _mm_dp_ps(d,e,0xF1);
    _mm_store_ss(&m[3][2],f);

    e = _mm_setr_ps(m[0][3], m[1][3], m[2][3], m[3][3]);
    f = _mm_dp_ps(a,e,0xF1);
    _mm_store_ss(&m[0][3],f);

    f = _mm_dp_ps(b,e,0xF1);
    _mm_store_ss(&m[1][3],f);

    f = _mm_dp_ps(c,e,0xF1);
    _mm_store_ss(&m[2][3],f);

    f = _mm_dp_ps(d,e,0xF1);
    _mm_store_ss(&m[3][3],f);

    return *this;
  }
#elif defined(SSE2)
  friend __forceinline Mat<Type,4> operator * (Mat<Type,4>& M0, Mat<Type,4>& M1) //SSE/SSE2, 16 mult. and 4 add vs 64 mult. and 32 add
  {
		__m128 a,b,c,d,e,f,g;
    Mat<Type,4> m;

		a=_mm_loadu_ps(&(M1[0][0]));
    b=_mm_loadu_ps(&(M1[1][0]));
    c=_mm_loadu_ps(&(M1[2][0]));
    d=_mm_loadu_ps(&(M1[3][0]));


    f=_mm_set_ps1(M0[0][0]);
    e=_mm_mul_ps(f,a);

    f=_mm_set_ps1(M0[0][1]);
    f=_mm_mul_ps(f,b);

    g=_mm_add_ps(e,f);


    f=_mm_set_ps1(M0[0][2]);
    e=_mm_mul_ps(f,c);

    f=_mm_set_ps1(M0[0][3]);
    f=_mm_mul_ps(f,d);

    _mm_storeu_ps(&(m[0][0]),_mm_add_ps(g,_mm_add_ps(e,f)));

    f=_mm_set_ps1(M0[1][0]);
    e=_mm_mul_ps(f,a);

    f=_mm_set_ps1(M0[1][1]);
    f=_mm_mul_ps(f,b);

    g=_mm_add_ps(e,f);


    f=_mm_set_ps1(M0[1][2]);
    e=_mm_mul_ps(f,c);

    f=_mm_set_ps1(M0[1][3]);
    f=_mm_mul_ps(f,d);

    _mm_storeu_ps(&(m[0][1]),_mm_add_ps(g,_mm_add_ps(e,f)));

    f=_mm_set_ps1(M0[2][0]);
    e=_mm_mul_ps(f,a);

    f=_mm_set_ps1(M0[2][1]);
    f=_mm_mul_ps(f,b);

    g=_mm_add_ps(e,f);


    f=_mm_set_ps1(M0[2][2]);
    e=_mm_mul_ps(f,c);

    f=_mm_set_ps1(M0[2][3]);
    f=_mm_mul_ps(f,d);

    _mm_storeu_ps(&(m[0][2]),_mm_add_ps(g,_mm_add_ps(e,f)));

    f=_mm_set_ps1(M0[3][0]);
    e=_mm_mul_ps(f,a);

    f=_mm_set_ps1(M0[3][1]);
    f=_mm_mul_ps(f,b);

    g=_mm_add_ps(e,f);


    f=_mm_set_ps1(M0[3][2]);
    e=_mm_mul_ps(f,c);

    f=_mm_set_ps1(M0[3][3]);
    f=_mm_mul_ps(f,d);

    _mm_storeu_ps(&(m[0][3]),_mm_add_ps(g,_mm_add_ps(e,f)));

    return m;
  }

  __forceinline Mat<Type,4>& operator *= (Mat<Type,4>& M0) //16 mult. und 4 add gegen 64 mult. und 32 add
  {
		__m128 a,b,c,d,e,f,g;

		a=_mm_loadu_ps(&(m[0][0]));
    b=_mm_loadu_ps(&(m[0][1]));
    c=_mm_loadu_ps(&(m[0][2]));
    d=_mm_loadu_ps(&(m[0][3]));


    f=_mm_set_ps1(M0[0][0]);
    e=_mm_mul_ps(f,a);

    f=_mm_set_ps1(M0[0][1]);
    f=_mm_mul_ps(f,b);

    g=_mm_add_ps(e,f);


    f=_mm_set_ps1(M0[0][2]);
    e=_mm_mul_ps(f,c);

    f=_mm_set_ps1(M0[0][3]);
    f=_mm_mul_ps(f,d);

    _mm_storeu_ps(&(m[0][0]),_mm_add_ps(g,_mm_add_ps(e,f)));

    f=_mm_set_ps1(M0[1][0]);
    e=_mm_mul_ps(f,a);

    f=_mm_set_ps1(M0[1][1]);
    f=_mm_mul_ps(f,b);

    g=_mm_add_ps(e,f);


    f=_mm_set_ps1(M0[1][2]);
    e=_mm_mul_ps(f,c);

    f=_mm_set_ps1(M0[1][3]);
    f=_mm_mul_ps(f,d);

    _mm_storeu_ps(&(m[0][1]),_mm_add_ps(g,_mm_add_ps(e,f)));

    f=_mm_set_ps1(M0[2][0]);
    e=_mm_mul_ps(f,a);

    f=_mm_set_ps1(M0[2][1]);
    f=_mm_mul_ps(f,b);

    g=_mm_add_ps(e,f);


    f=_mm_set_ps1(M0[2][2]);
    e=_mm_mul_ps(f,c);

    f=_mm_set_ps1(M0[2][3]);
    f=_mm_mul_ps(f,d);

    _mm_storeu_ps(&(m[0][2]),_mm_add_ps(g,_mm_add_ps(e,f)));

    f=_mm_set_ps1(M0[3][0]);
    e=_mm_mul_ps(f,a);

    f=_mm_set_ps1(M0[3][1]);
    f=_mm_mul_ps(f,b);

    g=_mm_add_ps(e,f);


    f=_mm_set_ps1(M0[3][2]);
    e=_mm_mul_ps(f,c);

    f=_mm_set_ps1(M0[3][3]);
    f=_mm_mul_ps(f,d);

    _mm_storeu_ps(&(m[0][3]),_mm_add_ps(g,_mm_add_ps(e,f)));

    return *this;
  }
#endif

  private:

  Vec<Type,4> m[4];
};




template<typename Type> class Mat<Type, 3>
{
  public:

  Mat()
  {}

  Mat(const Mat<Type,3>& M)
  {
    m[0] = M.m[0];
    m[1] = M.m[1];
    m[2] = M.m[2];
  }


  Mat(Vec<Type,3>& v0, Vec<Type,3>& v1, Vec<Type,3>& v2)
  {
    m[0][0]=v0[0]; m[1][0]=v1[0]; m[2][0]=v2[0];
    m[0][1]=v0[1]; m[1][1]=v1[1]; m[2][1]=v2[1];
    m[0][2]=v0[2]; m[1][2]=v1[2]; m[2][2]=v2[2];
  }

  Mat(const Type *a)
  {
    for(int i = 0; i < 3; i++)
    {
      for(int j = 0; j < 3; j++)
      {
        m[i][j] = a[4 * i + j];
      }
    }
  }

  Mat( Type m_11, Type m_21, Type m_31,
        Type m_12, Type m_22, Type m_32,
        Type m_13, Type m_23, Type m_33)
  {
    m[0][0]=m_11; m[1][0]=m_21; m[2][0]=m_31;
    m[0][1]=m_12; m[1][1]=m_22; m[2][1]=m_32;
    m[0][2]=m_13; m[1][2]=m_23; m[2][2]=m_33;
  }

	~Mat(){}

  static __forceinline Mat identity()
  {
    return Mat<Type,3>(1.0f,  0.0f,  0.0f,
                        0.0f,  1.0f,  0.0f,
                        0.0f,  0.0f,  1.0f);
  }

  __forceinline Vec<Type,3>& operator [](unsigned int i)
	{
	  return m[i];
	}

	__forceinline  Type detMat(void)
	{
	  return m[0][0]*(m[1][1]*m[2][2]-m[2][1]*m[1][2])+
            m[0][1]*(m[1][2]*m[2][0]-m[2][2]*m[1][0])+
            m[0][2]*(m[1][0]*m[2][1]-m[1][1]*m[2][0]);
	}

	__forceinline  Mat invert(void)
	{
		Type invDetM=detMat();
		Mat invM;
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

  __forceinline  Mat transpose(void)
  {
    return Mat<Type,3>(m[0][0], m[0][1], m[0][2],
                        m[1][0], m[1][1], m[1][2],
                        m[2][0], m[2][1], m[2][2]);
  }

#if defined(SSE4)
  __forceinline  Vec<Type,3> operator * ( Vec<Type,3>& v )//SSE4.1
  {
    __m128 a,b,c;
    Vec<Type,3> vTmp, vErg;
    b=_mm_loadu_ps(&v[0]);

    a=_mm_setr_ps(m[0][0], m[1][0], m[2][0], 0);
    c=_mm_dp_ps(a,b,0xF1);
    _mm_storeu_ps(&vErg[0],c);

    a=_mm_setr_ps(m[0][1], m[1][1], m[2][1], 0);
    c=_mm_dp_ps(a,b,0xF2);
    _mm_storeu_ps(&vTmp[0],c);
    vErg[1] = vTmp[1];

    a=_mm_setr_ps(m[0][2], m[1][2], m[2][2], 0);
    c=_mm_dp_ps(a,b,0xF4);
    _mm_storeu_ps(&vTmp[0],c);
    vErg[2] = vTmp[2];

    return vErg;
  }
#endif

  __forceinline  Mat operator * (const Type s )
  {
    __m128 a,b,c;
    Vec<Type,4> vTmp;
    Mat erg;
    Vec<Type,4> v(s, s, s, 0);
    b=_mm_loadu_ps(&v[0]);

    a=_mm_loadu_ps(&(m[0][0]));
    c=_mm_mul_ps(a,b);
    _mm_storeu_ps(&(vTmp[0]),c);
    erg[0] = Vec<Type,3>(vTmp[0],vTmp[1],vTmp[2]);

    a=_mm_loadu_ps(&(m[1][0]));
    c=_mm_mul_ps(a,b);
    _mm_storeu_ps(&(vTmp[0]),c);
    erg[1] = Vec<Type,3>(vTmp[0],vTmp[1],vTmp[2]);

    a=_mm_loadu_ps(&(m[2][0]));
    c=_mm_mul_ps(a,b);
    _mm_storeu_ps(&(vTmp[0]),c);
    erg[2] = Vec<Type,3>(vTmp[0],vTmp[1],vTmp[2]);

    return m;
  }

    __forceinline  Mat& operator *= ( Type s )
  {
    __m128 a,b,c;
    Vec<Type,4> vTmp;
    Vec<Type,4> v(s, s, s, 0);
    b=_mm_loadu_ps(&v[0]);

    a=_mm_loadu_ps(&(m[0][0]));
    c=_mm_mul_ps(a,b);
    _mm_storeu_ps(&(vTmp[0]),c);
    m[0] = Vec<Type,3>(vTmp[0],vTmp[1],vTmp[2]);

    a=_mm_loadu_ps(&(m[1][0]));
    c=_mm_mul_ps(a,b);
    _mm_storeu_ps(&(vTmp[0]),c);
    m[1] = Vec<Type,3>(vTmp[0],vTmp[1],vTmp[2]);

    a=_mm_loadu_ps(&(m[2][0]));
    c=_mm_mul_ps(a,b);
    _mm_storeu_ps(&(vTmp[0]),c);
    m[2] = Vec<Type,3>(vTmp[0],vTmp[1],vTmp[2]);

    return *this;
  }

  __forceinline  Mat operator + (const Mat& M)
  {
		__m128 a,b,c;
    Mat erg;

    a=_mm_loadu_ps(&(m[0][0]));
    b=_mm_loadu_ps(M[0]);
    c=_mm_add_ps(a,b);
    _mm_storeu_ps(&(erg[0][0]),c);

    a=_mm_loadu_ps(&(m[1][0]));
    b=_mm_loadu_ps(M[1]);
    c=_mm_add_ps(a,b);
    _mm_storeu_ps(&(erg[1][0]),c);

    a=_mm_loadu_ps(&(m[2][0]));
    b=_mm_loadu_ps(M[2]);
    c=_mm_add_ps(a,b);
    _mm_storeu_ps(&(erg[2][0]),c);

    return erg;
  }

  __forceinline  Mat operator - (const Mat& M)
  {
		__m128 a,b,c;
    Mat erg;

    a=_mm_loadu_ps(&(m[0][0]));
    b=_mm_loadu_ps(M[0]);
    c=_mm_sub_ps(a,b);
    _mm_storeu_ps(&(erg[0][0]),c);

    a=_mm_loadu_ps(&(m[1][0]));
    b=_mm_loadu_ps(M[1]);
    c=_mm_sub_ps(a,b);
    _mm_storeu_ps(&(erg[1][0]),c);

    a=_mm_loadu_ps(&(m[2][0]));
    b=_mm_loadu_ps(M[2]);
    c=_mm_sub_ps(a,b);
    _mm_storeu_ps(&(erg[2][0]),c);

    return erg;
  }

  __forceinline const Mat& operator += (const Mat& M)
  {
		__m128 a,b,c;
    a=_mm_loadu_ps(&(m[0][0]));
    b=_mm_loadu_ps(M[0]);
    c=_mm_sub_ps(a,b);
    _mm_storeu_ps(&(m[0][0]),c);

    a=_mm_loadu_ps(&(m[1][0]));
    b=_mm_loadu_ps(M[1]);
    c=_mm_sub_ps(a,b);
    _mm_storeu_ps(&(m[1][0]),c);

    a=_mm_loadu_ps(&(m[2][0]));
    b=_mm_loadu_ps(M[2]);
    c=_mm_sub_ps(a,b);
    _mm_storeu_ps(&(m[2][0]),c);

    return *this;
  }

  __forceinline const Mat& operator -= (const Mat& M)
  {
		__m128 a,b,c;
    a=_mm_loadu_ps(&(m[0][0]));
    b=_mm_loadu_ps(M[0]);
    c=_mm_sub_ps(a,b);
    _mm_storeu_ps(&(m[0][0]),c);

    a=_mm_loadu_ps(&(m[0][1]));
    b=_mm_loadu_ps(M[1]);
    c=_mm_sub_ps(a,b);
    _mm_storeu_ps(&(m[0][1]),c);

    a=_mm_loadu_ps(&(m[0][2]));
    b=_mm_loadu_ps(M[2]);
    c=_mm_sub_ps(a,b);
    _mm_storeu_ps(&(m[0][2]),c);

    return *this;
  }

#if defined(SSE4)
  friend __forceinline Mat<Type,3> operator * (Mat<Type,3>& M0, Mat<Type,3>& M1) //SSE4, 16 dot protucts vs 64 mult. and 32 add
  {
		__m128 a,b,c,e,f;
    Mat<Type,3> m;

		a=_mm_loadu_ps(&(M1[0][0]));
    b=_mm_loadu_ps(&(M1[1][0]));
    c=_mm_loadu_ps(&(M1[2][0]));

    e = _mm_setr_ps(M0[0][0], M0[1][0], M0[2][0], 0);
    f = _mm_dp_ps(a,e,0xF1);
    _mm_store_ss(&m[0][0],f);

    f = _mm_dp_ps(b,e,0xF1);
    _mm_store_ss(&m[1][0],f);

    f = _mm_dp_ps(c,e,0xF1);
    _mm_store_ss(&m[2][0],f);

    e = _mm_setr_ps(M0[0][1], M0[1][1], M0[2][1], 0);
    f = _mm_dp_ps(a,e,0xF1);
    _mm_store_ss(&m[0][1],f);

    f = _mm_dp_ps(b,e,0xF1);
    _mm_store_ss(&m[1][1],f);

    f = _mm_dp_ps(c,e,0xF1);
    _mm_store_ss(&m[2][1],f);

    e = _mm_setr_ps(M0[0][2], M0[1][2], M0[2][2], 0);
    f = _mm_dp_ps(a,e,0xF1);
    _mm_store_ss(&m[0][2],f);

    f = _mm_dp_ps(b,e,0xF1);
    _mm_store_ss(&m[1][2],f);

    f = _mm_dp_ps(c,e,0xF1);
    _mm_store_ss(&m[2][2],f);

    return m;
  }

  __forceinline const Mat<Type,3>& operator *= (Mat<Type,3>& M0) //SSE4, 16 dot protucts vs 64 mult. and 32 add
  {
		__m128 a,b,c,e,f;

		a=_mm_loadu_ps(&(M0[0][0]));
    b=_mm_loadu_ps(&(M0[1][0]));
    c=_mm_loadu_ps(&(M0[2][0]));

    e = _mm_setr_ps(m[0][0], m[1][0], m[2][0], 0);
    f = _mm_dp_ps(a,e,0xF1);
    _mm_store_ss(&m[0][0],f);

    f = _mm_dp_ps(b,e,0xF1);
    _mm_store_ss(&m[1][0],f);

    f = _mm_dp_ps(c,e,0xF1);
    _mm_store_ss(&m[2][0],f);

    e = _mm_setr_ps(m[0][1], m[1][1], m[2][1], 0);
    f = _mm_dp_ps(a,e,0xF1);
    _mm_store_ss(&m[0][1],f);

    f = _mm_dp_ps(b,e,0xF1);
    _mm_store_ss(&m[1][1],f);

    f = _mm_dp_ps(c,e,0xF1);
    _mm_store_ss(&m[2][1],f);


    e = _mm_setr_ps(m[0][2], m[1][2], m[2][2], 0);
    f = _mm_dp_ps(a,e,0xF1);
    _mm_store_ss(&m[0][2],f);

    f = _mm_dp_ps(b,e,0xF1);
    _mm_store_ss(&m[1][2],f);

      f = _mm_dp_ps(c,e,0xF1);
    _mm_store_ss(&m[2][2],f);


    return *this;
  }
#elif defined(SSE2)
  friend __forceinline Mat<Type,3> operator * (Mat<Type,3>& M0, Mat<Type,3>& M1) //16 mult. und 4 add gegen 64 mult. und 32 add
  {
		__m128 a,b,c,d,e,f,g;
    Mat<Type,3> m;

		a=_mm_loadu_ps(&(M1[0][0]));
    b=_mm_loadu_ps(&(M1[1][0]));
    c=_mm_loadu_ps(&(M1[2][0]));

    f=_mm_set_ps1(M0[0][0]);
    e=_mm_mul_ps(f,a);

    f=_mm_set_ps1(M0[0][1]);
    f=_mm_mul_ps(f,b);

    g=_mm_add_ps(e,f);


    f=_mm_set_ps1(M0[0][2]);
    e=_mm_mul_ps(f,c);

    f=_mm_set_ps1(M0[0][3]);
    f=_mm_mul_ps(f,d);

    _mm_storeu_ps(&(m[0][0]),_mm_add_ps(g,_mm_add_ps(e,f)));

    f=_mm_set_ps1(M0[1][0]);
    e=_mm_mul_ps(f,a);

    f=_mm_set_ps1(M0[1][1]);
    f=_mm_mul_ps(f,b);

    g=_mm_add_ps(e,f);


    f=_mm_set_ps1(M0[1][2]);
    e=_mm_mul_ps(f,c);

    f=_mm_set_ps1(M0[1][3]);
    f=_mm_mul_ps(f,d);

    _mm_storeu_ps(&(m[0][1]),_mm_add_ps(g,_mm_add_ps(e,f)));

    f=_mm_set_ps1(M0[2][0]);
    e=_mm_mul_ps(f,a);

    f=_mm_set_ps1(M0[2][1]);
    f=_mm_mul_ps(f,b);

    g=_mm_add_ps(e,f);


    f=_mm_set_ps1(M0[2][2]);
    e=_mm_mul_ps(f,c);

    f=_mm_set_ps1(M0[2][3]);
    f=_mm_mul_ps(f,d);

    _mm_storeu_ps(&(m[0][2]),_mm_add_ps(g,_mm_add_ps(e,f)));

    return m;
  }

  __forceinline Mat<Type,3> operator *= (Mat<Type,3>& M0) //16 mult. und 4 add gegen 64 mult. und 32 add
  {
		__m128 a,b,c,d,e,f,g;

		a=_mm_loadu_ps(&(m[0][0]));
    b=_mm_loadu_ps(&(m[0][1]));
    c=_mm_loadu_ps(&(m[0][2]));


    f=_mm_set_ps1(M0[0][0]);
    e=_mm_mul_ps(f,a);

    f=_mm_set_ps1(M0[0][1]);
    f=_mm_mul_ps(f,b);

    g=_mm_add_ps(e,f);


    f=_mm_set_ps1(M0[0][2]);
    e=_mm_mul_ps(f,c);

    f=_mm_set_ps1(M0[0][3]);
    f=_mm_mul_ps(f,d);

    _mm_storeu_ps(&(m[0][0]),_mm_add_ps(g,_mm_add_ps(e,f)));

    f=_mm_set_ps1(M0[1][0]);
    e=_mm_mul_ps(f,a);

    f=_mm_set_ps1(M0[1][1]);
    f=_mm_mul_ps(f,b);

    g=_mm_add_ps(e,f);


    f=_mm_set_ps1(M0[1][2]);
    e=_mm_mul_ps(f,c);

    f=_mm_set_ps1(M0[1][3]);
    f=_mm_mul_ps(f,d);

    _mm_storeu_ps(&(m[0][1]),_mm_add_ps(g,_mm_add_ps(e,f)));

    f=_mm_set_ps1(M0[2][0]);
    e=_mm_mul_ps(f,a);

    f=_mm_set_ps1(M0[2][1]);
    f=_mm_mul_ps(f,b);

    g=_mm_add_ps(e,f);


    f=_mm_set_ps1(M0[2][2]);
    e=_mm_mul_ps(f,c);

    f=_mm_set_ps1(M0[2][3]);
    f=_mm_mul_ps(f,d);

    _mm_storeu_ps(&(m[0][2]),_mm_add_ps(g,_mm_add_ps(e,f)));

    return *this;
  }
#endif

  private:

  Vec<Type,3> m[3];
};




template<typename Type> class Mat<Type,2>
{
  public:

  Mat()
  {}

  Mat(const Mat<Type,2>& M)
  {
    m[0] = M.m[0];
    m[1] = M.m[1];
  }


  Mat(Vec<Type,2>& v0, Vec<Type,2>& v1)
  {
    m[0][0]=v0[0];  m[1][0]=v0[1];
    m[0][1]=v1[0];  m[1][1]=v1[1];
  }

  Mat(const Type *a)
  {
    for(int i = 0; i < 2; i++)
    {
      for(int j = 0; j < 2; j++)
      {
        m[i][j] = a[4 * i + j];
      }
    }
  }

  Mat( Type m_11,  Type m_21,
        Type m_12,  Type m_22)
  {
    m[0][0]=m_11;  m[1][0]=m_21;
    m[0][1]=m_12;  m[1][1]=m_22;
  }

	~Mat(){}

  static __forceinline Mat identity()
  {
    return Mat<Type,2>(1.0f,  0.0f,
                        0.0f,  1.0f);
  }

  __forceinline Vec<Type,2>& operator [](unsigned int i)
	{
	  return m[i];
	}

	__forceinline Type detMat(void)
	{
	  return m[0][0] * m[1][1] - m[1][0] * m[0][1];
	}

	__forceinline Mat invert(void)
	{
		Type invDetM=detMat();
    Mat invM;

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

  __forceinline  Mat transpose(void)
  {
    return Mat<Type,2>(m[0][0], m[0][1],
                        m[1][0], m[1][1]);
  }

#if defined(SSE4)
  __forceinline Vec<Type,2> operator * ( Vec<Type,2>& v )//SSE4.1
  {
__m128 a,b,c;
    Vec<Type,2> vTmp, vErg;
    b=_mm_loadu_ps(&v[0]);

    a=_mm_setr_ps(m[0][0], m[1][0], 0, 0);
    c=_mm_dp_ps(a,b,0xF1);
    _mm_storeu_ps(&vErg[0],c);

    a=_mm_setr_ps(m[0][1], m[1][1], 0, 0);
    c=_mm_dp_ps(a,b,0xF2);
    _mm_storeu_ps(&vTmp[0],c);
    vErg[1] = vTmp[1];

    return vErg;
  }
#endif

    __forceinline  Mat operator * ( Type s )
  {
    __m128 a,b,c;
    Mat erg;
    Vec<Type,2> v(s, s);
    b=_mm_loadu_ps(&v[0]);

    a=_mm_loadu_ps(&(m[0][0]));
    c=_mm_mul_ps(a,b);
    _mm_storeu_ps(&(erg[0][0]),c);

    a=_mm_loadu_ps(&(m[1][0]));
    c=_mm_mul_ps(a,b);
    _mm_storeu_ps(&(erg[1][0]),c);

    return erg;
  }

  __forceinline const Mat& operator *= ( Type s )
  {
    __m128 a,b,c;
    Vec<Type,2> v(s, s);
    b=_mm_loadu_ps(&v[0]);

    a=_mm_loadu_ps(&(m[0][0]));
    c=_mm_mul_ps(a,b);
    _mm_storeu_ps(&(m[0][0]),c);

    a=_mm_loadu_ps(&(m[1][0]));
    c=_mm_mul_ps(a,b);
    _mm_storeu_ps(&(m[1][0]),c);

    return *this;
  }

  __forceinline Mat operator + (const Mat& M)
  {
		__m128 a,b,c;
    Mat erg;

    a=_mm_loadu_ps(&(m[0][0]));
    b=_mm_loadu_ps(M[0]);
    c=_mm_add_ps(a,b);
    _mm_storeu_ps(&(erg[0][0]),c);

    a=_mm_loadu_ps(&(m[1][0]));
    b=_mm_loadu_ps(M[1]);
    c=_mm_add_ps(a,b);
    _mm_storeu_ps(&(erg[1][0]),c);

    return erg;
  }

  __forceinline Mat operator - (const Mat& M)
  {
	  __m128 a,b,c;
    Mat erg;

    a=_mm_loadu_ps(&(m[0][0]));
    b=_mm_loadu_ps(M[0]);
    c=_mm_sub_ps(a,b);
    _mm_storeu_ps(&(erg[0][0]),c);

    a=_mm_loadu_ps(&(m[1][0]));
    b=_mm_loadu_ps(M[1]);
    c=_mm_sub_ps(a,b);
    _mm_storeu_ps(&(erg[1][0]),c);

    return erg;
  }

    __forceinline const Mat& operator += (const Mat& M)
    {
		__m128 a,b,c;

    a=_mm_loadu_ps(&(m[0][0]));
    b=_mm_loadu_ps(M[0]);
    c=_mm_add_ps(a,b);
    _mm_storeu_ps(&(m[0][0]),c);

    a=_mm_loadu_ps(&(m[1][0]));
    b=_mm_loadu_ps(M[1]);
    c=_mm_add_ps(a,b);
    _mm_storeu_ps(&(m[1][0]),c);

    return *this;
  }

  __forceinline  Mat& operator -= ( Mat& M)
  {
	  __m128 a,b,c;

    a=_mm_loadu_ps(&(m[0][0]));
    b=_mm_loadu_ps(M[0]);
    c=_mm_sub_ps(a,b);
    _mm_storeu_ps(&(m[0][0]),c);

    a=_mm_loadu_ps(&(m[1][0]));
    b=_mm_loadu_ps(M[1]);
    c=_mm_sub_ps(a,b);
    _mm_storeu_ps(&(m[1][0]),c);

    return *this;
  }


#if defined(SSE4)
  friend __forceinline Mat<Type,2> operator * (Mat<Type,2>& M0, Mat<Type,2>& M1) //SSE4, 16 dot protucts vs 64 mult. and 32 add
  {
		__m128 a,b,e,f;
    Mat<Type,2> m;

		a=_mm_loadu_ps(&(M1[0][0]));
    b=_mm_loadu_ps(&(M1[1][0]));

    e = _mm_setr_ps(M0[0][0], M0[1][0], 0, 0);
    f = _mm_dp_ps(a,e,0xF1);
    _mm_store_ss(&m[0][0],f);

    f = _mm_dp_ps(b,e,0xF1);
    _mm_store_ss(&m[1][0],f);


    e = _mm_setr_ps(M0[0][1], M0[1][1], 0, 0);
    f = _mm_dp_ps(a,e,0xF1);
    _mm_store_ss(&m[0][1],f);

    f = _mm_dp_ps(b,e,0xF1);
    _mm_store_ss(&m[1][1],f);



    return m;
  }

  __forceinline const Mat<Type,2>& operator *= ( Mat<Type,2>& M0) //SSE4, 16 dot protucts vs 64 mult. and 32 add
  {
		__m128 a,b,e,f;

    a=_mm_loadu_ps(&(M0[0][0]));
    b=_mm_loadu_ps(&(M0[1][0]));

    e = _mm_setr_ps(m[0][0], m[1][0], 0, 0);
    f = _mm_dp_ps(a,e,0xF1);
    _mm_store_ss(&m[0][0],f);

    f = _mm_dp_ps(b,e,0xF1);
    _mm_store_ss(&m[1][0],f);

    e = _mm_setr_ps(m[0][1], m[1][1], 0, 0);
    f = _mm_dp_ps(a,e,0xF1);
    _mm_store_ss(&m[0][1],f);

    f = _mm_dp_ps(b,e,0xF1);
    _mm_store_ss(&m[1][1],f);

    return *this;
  }
#elif defined(SSE2)
  friend __forceinline Mat<Type,2> operator * (const Mat<Type,2>& M0, const Mat<Type,2>& M1)
  {
    __m128 a,b,c,d,e,f,g;
    Mat<Type,2> m;

	  a=_mm_loadu_ps(&(M1[0][0]));
    b=_mm_loadu_ps(&(M1[1][0]));

    f=_mm_set_ps1(M0[0][0]);
    e=_mm_mul_ps(f,a);

    f=_mm_set_ps1(M0[0][1]);
    f=_mm_mul_ps(f,b);

    g=_mm_add_ps(e,f);


    f=_mm_set_ps1(M0[0][2]);
    e=_mm_mul_ps(f,c);

    f=_mm_set_ps1(M0[0][3]);
    f=_mm_mul_ps(f,d);

    _mm_storeu_ps(&(m[0][0]),_mm_add_ps(g,_mm_add_ps(e,f)));

    f=_mm_set_ps1(M0[1][0]);
    e=_mm_mul_ps(f,a);

    f=_mm_set_ps1(M0[1][1]);
    f=_mm_mul_ps(f,b);

    g=_mm_add_ps(e,f);


    f=_mm_set_ps1(M0[1][2]);
    e=_mm_mul_ps(f,c);

    f=_mm_set_ps1(M0[1][3]);
    f=_mm_mul_ps(f,d);

    _mm_storeu_ps(&(m[0][1]),_mm_add_ps(g,_mm_add_ps(e,f)));

    return m;
  }

  __forceinline Mat<Type,2>& operator *= (Mat<Type,2>& M0)
  {
    __m128 a,b,c,d,e,f,g;

	  a=_mm_loadu_ps(&(m[0][0]));
    b=_mm_loadu_ps(&(m[0][1]));


    f=_mm_set_ps1(M0[0][0]);
    e=_mm_mul_ps(f,a);

    f=_mm_set_ps1(M0[0][1]);
    f=_mm_mul_ps(f,b);

    g=_mm_add_ps(e,f);


    f=_mm_set_ps1(M0[0][2]);
    e=_mm_mul_ps(f,c);

    f=_mm_set_ps1(M0[0][3]);
    f=_mm_mul_ps(f,d);

    _mm_storeu_ps(&(m[0][0]),_mm_add_ps(g,_mm_add_ps(e,f)));

    f=_mm_set_ps1(M0[1][0]);
    e=_mm_mul_ps(f,a);

    f=_mm_set_ps1(M0[1][1]);
    f=_mm_mul_ps(f,b);

    g=_mm_add_ps(e,f);


    f=_mm_set_ps1(M0[1][2]);
    e=_mm_mul_ps(f,c);

    f=_mm_set_ps1(M0[1][3]);
    f=_mm_mul_ps(f,d);

    _mm_storeu_ps(&(m[0][1]),_mm_add_ps(g,_mm_add_ps(e,f)));

    return *this;
  }
#endif
private:

  Vec<Type,2> m[2];
};

#endif
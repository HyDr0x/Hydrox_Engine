namespace he
{
  template< typename TYPE, unsigned int DIM> class Vector;

  template< typename TYPE> class Vector<TYPE, VECTOR_NUM_ARGS>
  {
    public:

    Vector()	{m_vertexcounter++;}
    Vector(VECTOR_PARAMS)
    {
      VECTOR_INIT
      m_vertexcounter++;
    }
    Vector(const Vector<TYPE, VECTOR_NUM_ARGS>& v)
    { 
      VECTOR_INIT_VEC
      m_vertexcounter++; 
    }

    ~Vector(){m_vertexcounter--;};

    static Vector identity() { return Vector(VECTOR_IDENTITY); }

    unsigned int giveVertexNumber(void);

    inline const Vector& operator = (const Vector  &v) { VECTOR_ASSIGNEMENT_OP return *this; }

	  inline Vector operator + (const Vector &v) { return Vector( VECTOR_PLUS_OP ); }
    inline Vector operator - (const Vector &v) { return Vector( VECTOR_MINUS_OP ); }

    inline Vector operator * (const Vector &v) { return Vector( VECTOR_MULTIPLY_OP ); }
    inline Vector operator * (const TYPE &s) { return Vector( VECTOR_MULTIPLY_OP_SCALAR ); }

    inline Vector operator / (const Vector &v) { return Vector( VECTOR_DIVIDE_OP ); }
	  inline Vector operator / (const TYPE &s) { return Vector( VECTOR_DIVIDE_OP_SCALAR ); }

    inline const Vector& operator += (const Vector &v) { VECTOR_EQ_PLUS_OP return *this; }
    inline const Vector& operator -= (const Vector &v) { VECTOR_EQ_MINUS_OP return *this; }
    inline const Vector& operator *= (const Vector &v) { VECTOR_EQ_MULTIPLY_OP return *this; }
    inline const Vector& operator *= (const TYPE &s) { VECTOR_EQ_MULTIPLY_OP_SCALAR return *this; }
    inline const Vector& operator /= (const Vector &v) { VECTOR_EQ_DIVIDE_OP return *this; }
    inline const Vector& operator /= (const TYPE &s) { VECTOR_EQ_DIVIDE_OP_SCALAR return *this; }

    inline bool operator == (const Vector &v) { return VECTOR_COMP_EQ }
    inline bool operator != (const Vector &v) { return VECTOR_COMP_UQ }
	  inline bool operator >  (const Vector &v) { return VECTOR_COMP_GR }
    inline bool operator <  (const Vector &v) { return VECTOR_COMP_LS }
	  inline bool operator >= (const Vector &v) { return VECTOR_COMP_EQ_GR }
    inline bool operator <= (const Vector &v) { return VECTOR_COMP_EQ_LS }

    inline TYPE& operator [] (unsigned int i) { return m_x[i]; }

    inline double length() 
    { 
      double length = 0;

      for(unsigned int i = 0; i != VECTOR_NUM_ARGS; i++)
      {
        length += m_x[i] * m_x[i];
      }
      return sqrt(length); 
    }

    inline void normalize() 
    { 
      double l = 1.0 / length();

      for(unsigned int i = 0; i != VECTOR_NUM_ARGS; i++)
      {
        m_x[i] *= static_cast<float>(l);
      }
    }

    inline static TYPE dot(Vector<TYPE, VECTOR_NUM_ARGS> a, Vector<TYPE, VECTOR_NUM_ARGS> b)
    {
      TYPE sum = 0;

      for(unsigned int i = 0; i != VECTOR_NUM_ARGS; i++)
      {
        sum += a[i] * b[i];
      }

      return sum;
    }

    inline static Vector<TYPE, VECTOR_NUM_ARGS> lerp(Vector<TYPE, VECTOR_NUM_ARGS> a, Vector<TYPE, VECTOR_NUM_ARGS> b, TYPE t)
    {
      return a * (1.0f - t) + b * t;
    }

    inline const unsigned int dimension() { return VECTOR_NUM_ARGS; }

    private:

    TYPE m_x[VECTOR_NUM_ARGS];

    static unsigned int m_vertexcounter;
  };


  template< typename TYPE> unsigned int Vector<TYPE, VECTOR_NUM_ARGS>::m_vertexcounter=0;

  template< typename TYPE> unsigned int Vector<TYPE, VECTOR_NUM_ARGS>::giveVertexNumber(void)
  {
      return m_vertexcounter;
  }
}

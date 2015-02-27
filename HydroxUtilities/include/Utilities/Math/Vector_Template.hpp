#define MIN(a, b) a < b ? a : b

namespace he
{
  namespace util
  {
    template<typename Type, unsigned int Dim> class Vector;

    template<typename Type> class Vector<Type, VECTOR_NUM_ARGS>
    {
      public:

      Vector()  {m_vertexcounter++;}
      Vector(VECTOR_PARAMS)
      {
        VECTOR_INIT
        m_vertexcounter++;
      }

      Vector(const Vector<Type, VECTOR_NUM_ARGS>& v)
      { 
        VECTOR_INIT_VEC
        m_vertexcounter++; 
      }

      template<unsigned int Dim2> Vector(const Vector<Type, Dim2>& v)
      { 
        unsigned int b = MIN(VECTOR_NUM_ARGS, Dim2);
        for(unsigned int i = 0; i < b; i++)
        {
          m_x[i] = v[i];
        }

        m_vertexcounter++; 
      }

      ~Vector(){ m_vertexcounter--; }

      template<unsigned int Dim2> Vector<Type, VECTOR_NUM_ARGS>& operator=(const Vector<Type, Dim2>& v)
      {
        unsigned int b = MIN(VECTOR_NUM_ARGS, Dim2);
        for(unsigned int i = 0; i < b; i++)
        {
          m_x[i] = v[i];
        }

        m_vertexcounter++;

        return *this;
      }

      static Vector identity() { return Vector(VECTOR_IDENTITY); }

      unsigned int giveVertexNumber(void);

      inline Vector& operator = (const Vector  &v) { VECTOR_ASSIGNEMENT_OP return *this; }

      inline Vector operator + (const Vector &v) const { return Vector( VECTOR_PLUS_OP ); }
      inline Vector operator + (Type s) const { return Vector(VECTOR_PLUS_OP_SCALAR); }

      inline Vector operator - (const Vector &v) const { return Vector( VECTOR_MINUS_OP ); }
      inline Vector operator - (Type s) const { return Vector(VECTOR_MINUS_OP_SCALAR); }

      inline Vector operator * (const Vector &v) const { return Vector( VECTOR_MULTIPLY_OP ); }
      inline Vector operator * (Type s) const { return Vector( VECTOR_MULTIPLY_OP_SCALAR ); }

      inline Vector operator / (const Vector &v) const { return Vector( VECTOR_DIVIDE_OP ); }
      inline Vector operator / (Type s) const { return Vector( VECTOR_DIVIDE_OP_SCALAR ); }

      inline Vector& operator += (const Vector &v) { VECTOR_EQ_PLUS_OP return *this; }
      inline Vector& operator += (Type s) { VECTOR_EQ_PLUS_OP_SCALAR return *this; }

      inline Vector& operator -= (const Vector &v) { VECTOR_EQ_MINUS_OP return *this; }
      inline Vector& operator -= (Type s) { VECTOR_EQ_MINUS_OP_SCALAR return *this; }

      inline Vector& operator *= (const Vector &v) { VECTOR_EQ_MULTIPLY_OP return *this; }
      inline Vector& operator *= (Type s) { VECTOR_EQ_MULTIPLY_OP_SCALAR return *this; }

      inline Vector& operator /= (const Vector &v) { VECTOR_EQ_DIVIDE_OP return *this; }
      inline Vector& operator /= (Type s) { VECTOR_EQ_DIVIDE_OP_SCALAR return *this; }

      inline bool operator == (const Vector &v) const { return VECTOR_COMP_EQ }
      inline bool operator != (const Vector &v) const { return VECTOR_COMP_UQ }
      inline bool operator >  (const Vector &v) const { return VECTOR_COMP_GR }
      inline bool operator <  (const Vector &v) const { return VECTOR_COMP_LS }
      inline bool operator >= (const Vector &v) const { return VECTOR_COMP_EQ_GR }
      inline bool operator <= (const Vector &v) const { return VECTOR_COMP_EQ_LS }

      inline Vector& operator & (const Vector &v) const { return Vector(VECTOR_BINOP_EQ); }
      inline Vector& operator | (const Vector &v) const { return Vector(VECTOR_BINOP_OR); }
      inline Vector& operator ^  (const Vector &v) const { return Vector(VECTOR_BINOP_XOR); }
      inline Vector& operator &=  (const Vector &v) { VECTOR_BINOP_EQ_EQ return *this; }
      inline Vector& operator |= (const Vector &v) { VECTOR_BINOP_EQ_OR return *this; }
      inline Vector& operator ^= (const Vector &v) { VECTOR_BINOP_EQ_XOR return *this; }

      inline Type& operator [] (unsigned int i) { return m_x[i]; }
      inline const Type& operator [] (unsigned int i) const { return m_x[i]; }

      inline float length() const
      { 
        float length = 0;

        for(unsigned int i = 0; i != VECTOR_NUM_ARGS; i++)
        {
          length += m_x[i] * m_x[i];
        }
        return static_cast<float>(sqrt(length)); 
      }

      inline Vector normalize() const
      { 
        Vector<Type, VECTOR_NUM_ARGS> result;

        double l = 1.0 / (length() + FLT_MIN);

        for(unsigned int i = 0; i != VECTOR_NUM_ARGS; i++)
        {
          result[i] = m_x[i] * static_cast<float>(l);
        }

        return result;
      }

      inline static Type dot(const Vector<Type, VECTOR_NUM_ARGS>& a, const Vector<Type, VECTOR_NUM_ARGS>& b)
      {
        Type sum = 0;

        for(unsigned int i = 0; i != VECTOR_NUM_ARGS; i++)
        {
          sum += a[i] * b[i];
        }

        return sum;
      }

      inline static Vector lerp(const Vector<Type, VECTOR_NUM_ARGS>& a, const Vector<Type, VECTOR_NUM_ARGS>& b, float t)
      {
        return a * (1.0f - t) + b * t;
      }

      inline const unsigned int dimension() { return VECTOR_NUM_ARGS; }

      private:

      Type m_x[VECTOR_NUM_ARGS];

      static unsigned int m_vertexcounter;
    };

    template<typename Type> Vector<Type, VECTOR_NUM_ARGS> operator + (Type s, const Vector<Type, VECTOR_NUM_ARGS>& v) { return Vector<Type, VECTOR_NUM_ARGS>(VECTOR_PLUS_OP_SCALAR_LEFT); }
    template<typename Type> Vector<Type, VECTOR_NUM_ARGS> operator - (Type s, const Vector<Type, VECTOR_NUM_ARGS>& v) { return Vector<Type, VECTOR_NUM_ARGS>(VECTOR_MINUS_OP_SCALAR_LEFT); }
    template<typename Type> Vector<Type, VECTOR_NUM_ARGS> operator * (Type s, const Vector<Type, VECTOR_NUM_ARGS>& v) { return Vector<Type, VECTOR_NUM_ARGS>(VECTOR_MULTIPLY_OP_SCALAR_LEFT); }
    template<typename Type> Vector<Type, VECTOR_NUM_ARGS> operator / (Type s, const Vector<Type, VECTOR_NUM_ARGS>& v) { return Vector<Type, VECTOR_NUM_ARGS>(VECTOR_DIVIDE_OP_SCALAR_LEFT); }

    template<typename Type> std::ostream& operator <<(std::ostream& stream, const Vector<Type, VECTOR_NUM_ARGS>& v)
    {
      for(unsigned int i = 0; i < VECTOR_NUM_ARGS; i++)
      {
        stream << v[i];
        if(i + 1 != VECTOR_NUM_ARGS)
        {
          stream << ',';
        }
      }

      return stream;
    }

    template<typename Type> std::istream& operator >>(std::istream& stream, Vector<Type, VECTOR_NUM_ARGS>& v)
    {
      for(unsigned int i = 0; i < VECTOR_NUM_ARGS; i++)
      {
        stream >> v[i];
        stream.ignore(1, ',');//ignores the next char or  ','
      }

      return stream;
    }

    template<typename Type> unsigned int Vector<Type, VECTOR_NUM_ARGS>::m_vertexcounter = 0;

    template<typename Type> unsigned int Vector<Type, VECTOR_NUM_ARGS>::giveVertexNumber()
    {
      return m_vertexcounter;
    }
  }
}

#undef MIN
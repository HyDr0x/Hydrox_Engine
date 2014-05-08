#ifndef FLAGS_HPP_
#define FLAGS_HPP_

namespace he
{
  namespace util
  {
    template<class ENUM> class Flags
    {
    public:

      typedef unsigned int int_t;

      explicit Flags() : m_flag(static_cast<ENUM>(0))
      {
      }

      explicit Flags(ENUM flag) : m_flag(flag)
      {
      }

      explicit Flags(int_t flag) : m_flag(static_cast<ENUM>(flag))
      {
      }

      Flags(const Flags& other)
      {
        m_flag = other.m_flag;
      }

      Flags& operator=(const Flags& other)
      {
        m_flag = other.m_flag;

        return *this;
      }

      int_t toInt() const
      {
        return static_cast<int_t>(m_flag);
      }

      Flags operator&(const Flags& other)
      {
        return Flags(toInt() & other.toInt);
      }

      Flags operator|(const Flags& other)
      {
        return Flags(toInt() | other.toInt);
      }

      Flags operator^(const Flags& other)
      {
        return Flags(toInt() ^ other.toInt);
      }

      Flags operator~()
      {
        return Flags(~toInt());
      }

      Flags& operator&=(const Flags& other)
      {
        return *this = (*this & other);
      }

      Flags& operator|=(const Flags& other)
      {
        return *this = (*this | other);
      }

      Flags& operator^=(const Flags& other)
      {
        return *this = (*this ^ other);
      }

    private:

      ENUM m_flag;
    };
  }
}

#endif
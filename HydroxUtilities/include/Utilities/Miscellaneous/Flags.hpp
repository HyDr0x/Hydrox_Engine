#ifndef FLAGS_HPP_
#define FLAGS_HPP_

#include <cstdint>

namespace he
{
  namespace util
  {
    template<class ENUM> class Flags
    {
    public:

      //typedef unsigned int int_t;//needs to be 32bit, to get the number of raised flags correctyl calculated

      static Flags<ENUM> convertToFlag(ENUM flag)
      {
        return Flags<ENUM>(static_cast<uint32_t>(pow(2.0f, static_cast<int>(flag))));
      }

      static Flags<ENUM> convertToFlag(uint32_t flag)
      {
        return Flags<ENUM>(static_cast<uint32_t>(pow(2.0f, static_cast<int>(flag))));
      }

      explicit Flags() : m_flag(static_cast<ENUM>(0))
      {
      }

      explicit Flags(ENUM flag)
      {
        m_flag = flag;
      }

      explicit Flags(uint32_t flag)
      {
        
        m_flag = static_cast<ENUM>(flag);
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

      uint32_t toInt()
      {
        return static_cast<uint32_t>(m_flag);
      }

      Flags operator&(const Flags& other)
      {
        return Flags(toInt() & other.toInt());
      }

      Flags operator|(const Flags& other)
      {
        return Flags(toInt() | other.toInt());
      }

      Flags operator^(const Flags& other)
      {
        return Flags(toInt() ^ other.toInt());
      }

      uint32_t toInt() const
      {
        return static_cast<uint32_t>(m_flag);
      }

      Flags operator&(const Flags& other) const
      {
        return Flags(toInt() & other.toInt());
      }

      Flags operator|(const Flags& other) const
      {
        return Flags(toInt() | other.toInt());
      }

      Flags operator^(const Flags& other) const
      {
        return Flags(toInt() ^ other.toInt());
      }

      Flags operator~() const
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

      bool operator==(const Flags& other) const
      {
        return this->toInt() == other.toInt();
      }

      bool operator!=(const Flags& other) const
      {
        return this->toInt() != other.toInt();
      }

      uint32_t raisedFlagNumber() const
      {
        uint32_t i = m_flag;
        i = i - ((i >> 1) & 0x55555555);
        i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
        return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
      }

    private:

      ENUM m_flag;
    };
  }
}

#endif
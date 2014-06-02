#include "Utilities/Pointer/WeakPointer.h"

//#include "Utilities/Pointer/SharedPointer.h"

namespace he
{
	namespace util
	{
    template<typename T> WeakPointer<T>::WeakPointer()
    {
      m_ptr = nullptr;
      m_referenceNumber = nullptr;
    }

	  template<typename T> WeakPointer<T>::WeakPointer(const WeakPointer<T>& other) : m_referenceNumber(other.m_referenceNumber), m_ptr(other.m_ptr)
	  {
	  }

    template<typename T> WeakPointer<T>::WeakPointer(const SharedPointer<T>& other) : m_referenceNumber(other.m_referenceNumber), m_ptr(other.m_ptr)
	  {
	  }

    template<typename T> WeakPointer<T>::WeakPointer& operator=(WeakPointer<T> other)
	  {
      if(*this == other)
      {
        return *this;
      }

      swap(other);

		  return *this;
	  }

    template<typename T> WeakPointer<T>::WeakPointer& operator=(SharedPointer<T> other)
	  {
      if(*this == other)
      {
        return *this;
      }

      swap(other);

		  return *this;
	  }

    template<typename T> void WeakPointer<T>::reset()
    {
      m_ptr = nullptr;
      m_referenceNumber = nullptr;
    }

    template<typename T> template<typename F> void WeakPointer<T>::reset(F *ptr)
    {
      WeakPointer<T> other = static_cast<T*>(ptr);
      swap(other);
    }

    template<typename T> WeakPointer<T>::operator bool() const
    {
      return m_ptr != nullptr;
    }

    template<typename T> SharedPointer<T> WeakPointer<T>::lock() const
    {
      return bool(*this) ? SharedPointer<T>(*this) : SharedPointer<T>();
    }

    template<typename T> unsigned int WeakPointer<T>::use_count() const
    {
      return m_referenceNumber != nullptr ? *m_referenceNumber : 0;
    }

    template<typename T> void WeakPointer<T>::swap(WeakPointer<T>& other)
    {
      std::swap(m_referenceNumber, other.m_referenceNumber);
      std::swap(m_ptr, other.m_ptr);
    }
	}
}
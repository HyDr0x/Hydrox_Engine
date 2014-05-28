#include "Utilities/Pointer/SharedPointer.h"

namespace he
{
	namespace util
	{
    template<typename T> SharedPointer<T>::SharedPointer()
    {
      m_ptr = nullptr;
      m_referenceNumber = nullptr;
    }

	  template<typename T> explicit SharedPointer<T>::SharedPointer(T* obj) : m_ptr(obj), m_referenceNumber(new unsigned int(1))
	  {}

	  template<typename T> SharedPointer<T>::SharedPointer(const SharedPointer<T>& other) : m_referenceNumber(other.m_referenceNumber), m_ptr(other.m_ptr)
	  {
      if(m_referenceNumber != nullptr)
      {
		    (*m_referenceNumber)++;
      }
	  }

    template<typename T> SharedPointer<T>::SharedPointer(const WeakPointer<T>& other) : m_referenceNumber(other.m_referenceNumber), m_ptr(other.m_ptr)
	  {
      if(m_referenceNumber != nullptr)
      {
		    (*m_referenceNumber)++;
      }
	  }

	  template<typename T> SharedPointer<T>::~SharedPointer()
	  {
		  ownDelete();
	  }

    template<typename T> SharedPointer<T>::SharedPointer& operator=(SharedPointer<T> other)
	  {
      if(*this == other)
      {
        return *this;
      }

      swap(other);

		  return *this;
	  }

    template<typename T> bool SharedPointer<T>::operator==(const SharedPointer& other) const
    {
      return m_referenceNumber == other.m_referenceNumber && m_ptr == other.m_ptr;
    }

    template<typename T, typename F> SharedPointer<F> SharedPointer<T>::static_pointer_cast() const
    {
      SharedPointer<F> other = SharedPointer<F>(static_cast<F*>(m_ptr));
      other.m_referenceNumber = m_referenceNumber;
      (*m_referenceNumber)++;
      return other;
    }

    template<typename T, typename F> SharedPointer<F> SharedPointer<T>::dynamic_pointer_cast() const
    {
      SharedPointer<F> other = SharedPointer<F>(dynamic_cast<F*>(m_ptr));
      other.m_referenceNumber = m_referenceNumber;
      (*m_referenceNumber)++;
      return other;
    }

    template<typename T> void SharedPointer<T>::release()
    {
      delete m_ptr;
      delete m_referenceNumber;
      m_ptr = nullptr;
      m_referenceNumber = nullptr;
    }

    template<typename T> void SharedPointer<T>::reset()
    {
      ownDelete();
    }

    template<typename T, typename F> void SharedPointer<T>::reset(F *ptr)
    {
      SharedPointer<T> other = static_cast<T*>(ptr);
      swap(other);
    }

    template<typename T> SharedPointer<T>::operator bool() const
    {
      return m_ptr != nullptr;
    }

    template<typename T> unsigned int SharedPointer<T>::use_count() const
    {
      return m_referenceNumber != nullptr ? *m_referenceNumber : 0;
    }

    template<typename T> T* SharedPointer<T>::get() const
    {
      return m_ptr;
    }

	  template<typename T> T* SharedPointer<T>::operator->() const
	  {
		  return m_ptr;
	  }

    template<typename T> T& SharedPointer<T>::operator*() const
	  {
		  return *m_ptr;
	  }

    template<typename T> void SharedPointer<T>::ownDelete()
    {
      if(m_referenceNumber != nullptr && --(*m_referenceNumber) == 0)
		  {
			  delete m_ptr;
			  delete m_referenceNumber;
        m_ptr = nullptr;
        m_referenceNumber = nullptr;
		  }
    }

    template<typename T> void SharedPointer<T>::swap(SharedPointer<T>& other)
    {
      std::swap(m_referenceNumber, other.m_referenceNumber);
      std::swap(m_ptr, other.m_ptr);
    }

    ////////////////////////////////////////////////////////////////////////////////////
    //                                                                                //
    //                             ARRAY SPECIALIZATION                               //
    //                                                                                //
    ////////////////////////////////////////////////////////////////////////////////////

    template<typename T> SharedPointer<T[]>::SharedPointer()
    {
      m_ptr = nullptr;
      m_referenceNumber = nullptr;
    }

	  template<typename T> explicit SharedPointer<T[]>::SharedPointer(T[] obj) : m_ptr(obj), m_referenceNumber(new unsigned int(1))
	  {}

	  template<typename T> SharedPointer<T[]>::SharedPointer(const SharedPointer<T[]>& other) : m_referenceNumber(other.m_referenceNumber), m_ptr(other.m_ptr)
	  {
      if(m_referenceNumber != nullptr)
      {
		    (*m_referenceNumber)++;
      }
	  }

    template<typename T> SharedPointer<T>::SharedPointer(const WeakPointer<T[]>& other) : m_referenceNumber(other.m_referenceNumber), m_ptr(other.m_ptr)
	  {
      if(m_referenceNumber != nullptr)
      {
		    (*m_referenceNumber)++;
      }
	  }

	  template<typename T> SharedPointer<T[]>::~SharedPointer()
	  {
		  ownDelete();
	  }

    template<typename T> SharedPointer<T[]>::SharedPointer& operator=(SharedPointer<T[]> other)
	  {
      if(*this == other)
      {
        return *this;
      }

      swap(other);

		  return *this;
	  }

    template<typename T> bool SharedPointer<T[]>::operator==(const SharedPointer& other) const
    {
      return m_referenceNumber == other.m_referenceNumber && m_ptr == other.m_ptr;
    }

    template<typename T, typename F> SharedPointer<F[]> SharedPointer<T[]>::static_pointer_cast() const
    {
      SharedPointer<F[]> other = SharedPointer<F[]>(static_cast<F[]>(m_ptr));
      other.m_referenceNumber = m_referenceNumber;
      (*m_referenceNumber)++;
      return other;
    }

    template<typename T, typename F> SharedPointer<F[]> SharedPointer<T[]>::dynamic_pointer_cast() const
    {
      SharedPointer<F[]> other = SharedPointer<F[]>(dynamic_cast<F[]>(m_ptr));
      other.m_referenceNumber = m_referenceNumber;
      (*m_referenceNumber)++;
      return other;
    }

    template<typename T> void SharedPointer<T[]>::release()
    {
      delete[] m_ptr;
      delete m_referenceNumber;
      m_ptr = nullptr;
      m_referenceNumber = nullptr;
    }

    template<typename T> void SharedPointer<T[]>::reset()
    {
      ownDelete();
    }

    template<typename T, typename F> void SharedPointer<T[]>::reset(F *ptr)
    {
      SharedPointer<T[]> other = static_cast<T*>(ptr);
      swap(other);
    }

    template<typename T> SharedPointer<T[]>::operator bool() const
    {
      return m_ptr != nullptr;
    }

    template<typename T> unsigned int SharedPointer<T[]>::use_count() const
    {
      return m_referenceNumber != nullptr ? *m_referenceNumber : 0;
    }

    template<typename T> T& SharedPointer<T[]>::operator[] (unsigned int i)
    {
      return m_ptr[i];
    }

    template<typename T> const T& SharedPointer<T[]>::operator[] (unsigned int i) const
    {
      return m_ptr[i];
    }

    template<typename T> T* SharedPointer<T[]>::get() const
    {
      return m_ptr;
    }

	  template<typename T> T* SharedPointer<T[]>::operator->() const
	  {
		  return m_ptr;
	  }

    template<typename T> T& SharedPointer<T[]>::operator*() const
	  {
		  return *m_ptr;
	  }

    template<typename T> void SharedPointer<T[]>::ownDelete()
    {
      if(m_referenceNumber != nullptr && --(*m_referenceNumber) == 0)
		  {
			  delete[] m_ptr;
			  delete m_referenceNumber;
        m_ptr = nullptr;
        m_referenceNumber = nullptr;
		  }
    }

    template<typename T> void SharedPointer<T[]>::swap(SharedPointer<T[]>& other)
    {
      std::swap(m_referenceNumber, other.m_referenceNumber);
      std::swap(m_ptr, other.m_ptr);
    }
	}
}
#include "Utilities/Pointer/UniquePointer.h"

namespace he
{
  namespace util
  {
    template<typename T> UniquePointer<T>::UniquePointer()
    {
      m_ptr = nullptr;
    }

    template<typename T> UniquePointer<T>::UniquePointer(T* obj) : m_ptr(obj)
    {}

    template<typename T> UniquePointer<T>::~UniquePointer()
    {
      delete m_ptr;
      m_ptr = nullptr;
    }

    template<typename T> void UniquePointer<T>::release()
    {
      delete m_ptr;
      m_ptr = nullptr;
    }

    template<typename T> void UniquePointer<T>::reset()
    {
      delete m_ptr;
      m_ptr = nullptr;
    }

    template<typename T> template<typename F> void UniquePointer<T>::reset(F *ptr)
    {
      UniquePointer<T> other = static_cast<T*>(ptr);
      swap(other);
    }

    template<typename T> UniquePointer<T>::operator bool() const
    {
      return m_ptr != nullptr;
    }

    template<typename T> T* UniquePointer<T>::get() const
    {
      return m_ptr;
    }

    template<typename T> T* UniquePointer<T>::operator->() const
    {
      return m_ptr;
    }

    template<typename T> T& UniquePointer<T>::operator*() const
    {
      return *m_ptr;
    }

    template<typename T> void UniquePointer<T>::swap(UniquePointer<T>& other)
    {
      std::swap(m_ptr, other.m_ptr);
    }

    ////////////////////////////////////////////////////////////////////////////////////
    //                                                                                //
    //                             ARRAY SPECIALIZATION                               //
    //                                                                                //
    ////////////////////////////////////////////////////////////////////////////////////

    template<typename T> UniquePointer<T[]>::UniquePointer()
    {
      m_ptr = nullptr;
    }

    template<typename T> UniquePointer<T[]>::UniquePointer(T[] obj) : m_ptr(obj)
    {}

    template<typename T> UniquePointer<T[]>::~UniquePointer()
    {
      delete[] m_ptr;
      m_ptr = nullptr;
    }

    template<typename T> void UniquePointer<T[]>::release()
    {
      delete[] m_ptr;
      m_ptr = nullptr;
    }

    template<typename T> void UniquePointer<T[]>::reset()
    {
      delete[] m_ptr;
      m_ptr = nullptr;
    }

    template<typename T> template<typename F> void UniquePointer<T[]>::reset(F *ptr)
    {
      UniquePointer<T[]> other = static_cast<T*>(ptr);
      swap(other);
    }

    template<typename T> UniquePointer<T[]>::operator bool() const
    {
      return m_ptr != nullptr;
    }

    template<typename T> T& UniquePointer<T[]>::operator[] (unsigned int i)
    {
      return m_ptr[i];
    }

    template<typename T> const T& UniquePointer<T[]>::operator[] (unsigned int i) const
    {
      return m_ptr[i];
    }

    template<typename T> T* UniquePointer<T[]>::get() const
    {
      return m_ptr;
    }

    template<typename T> void UniquePointer<T[]>::ownDelete()
    {
      delete[] m_ptr;
      m_ptr = nullptr;
    }

    template<typename T> void UniquePointer<T[]>::swap(UniquePointer<T[]>& other)
    {
      std::swap(m_ptr, other.m_ptr);
    }
  }
}
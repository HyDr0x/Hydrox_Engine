#ifndef UNIQUEPOINTER_HPP_
#define UNIQUEPOINTER_HPP_

namespace he
{
  namespace util
  {
    template<typename T> class UniquePointer
    {
    public:

      UniquePointer();
      explicit UniquePointer(T* obj);
      UniquePointer(UniquePointer<T>&& other);

      ~UniquePointer();

      UniquePointer& operator=(UniquePointer<T>&& other);

      void release();

      void reset();
      template<typename F> void reset(F *ptr);

      operator bool() const;

      T* get() const;

      T* operator->() const;
      T& operator*() const;

    private:

      UniquePointer(const UniquePointer<T>& other);
      const UniquePointer& operator=(const UniquePointer<T>& other);

      void swap(UniquePointer<T>& other);

      T *m_ptr;
    };

    template<typename T> class UniquePointer<T[]>
    {
    public:

      UniquePointer();
      explicit UniquePointer(T obj[]);
      UniquePointer(UniquePointer<T[]>& other);

      ~UniquePointer();

      UniquePointer<T[]>& operator=(UniquePointer<T[]>& other);

      void release();

      void reset();
      template<typename F> void reset(F ptr[]);

      operator bool() const;

      T& operator[] (unsigned int i);
      const T& operator[] (unsigned int i) const;

      T* get() const;

    private:

      void swap(UniquePointer<T[]>& other);

      T *m_ptr;
    };
  }
}

#include "UniquePointer.inl"

#endif

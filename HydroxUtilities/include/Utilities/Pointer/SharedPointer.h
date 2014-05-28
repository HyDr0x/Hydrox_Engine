#ifndef SHAREDPOINTER_HPP_
#define SHAREDPOINTER_HPP_

#include "Utilities/Pointer/WeakPointer.h"

namespace he
{
	namespace util
	{
    template<typename T> class SharedPointer
    {
    public:

      SharedPointer();
	    explicit SharedPointer(T* obj);
	    SharedPointer(const SharedPointer<T>& other);
      SharedPointer(const WeakPointer<T>& other);

	    ~SharedPointer();

      SharedPointer& operator=(SharedPointer<T> other);

      bool operator==(const SharedPointer& other) const;

      template<typename F> SharedPointer<F> static_pointer_cast() const;
      template<typename F> SharedPointer<F> dynamic_pointer_cast() const;

      void release();

      void reset();
      template<typename F> void reset(F *ptr);

      operator bool() const;

      unsigned int use_count() const;

      T* get() const;

	    T* operator->() const;
      T& operator*() const;

    private:

      void ownDelete();

      void swap(SharedPointer<T>& other);

	    unsigned int *m_referenceNumber;
	    T* m_ptr;
    };



    template<typename T> class SharedPointer<T[]>
    {
    public:

      SharedPointer();
	    explicit SharedPointer(T[] obj);
	    SharedPointer(const SharedPointer<T[]>& other);
      SharedPointer(const WeakPointer<T[]>& other);

	    ~SharedPointer();

      SharedPointer& operator=(SharedPointer<T[]> other);

      bool operator==(const SharedPointer& other) const;

      template<typename F> SharedPointer<F[]> static_pointer_cast() const;
      template<typename F> SharedPointer<F[]> dynamic_pointer_cast() const;

      void release();

      void reset();
      template<typename F> void reset(F[] *ptr);

      operator bool() const;

      unsigned int use_count() const;

      T& operator[] (unsigned int i);
      const T& operator[] (unsigned int i) const;

      T[] get() const;

	    T[] operator->() const;
      T[]& operator*() const;

    private:

      void ownDelete();

      void swap(SharedPointer<T[]>& other);

	    unsigned int *m_referenceNumber;
	    T* m_ptr;
    };
	}
}

#include "SharedPointer.inl"

#endif

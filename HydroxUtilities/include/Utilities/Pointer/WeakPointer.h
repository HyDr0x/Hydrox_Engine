#ifndef WEAKPOINTER_HPP_
#define WEAKPOINTER_HPP_

#include "Utilities/Pointer/SharedPointer.h"

namespace he
{
	namespace util
	{
    template<typename T> class WeakPointer
    {
    public:

      WeakPointer();
	    WeakPointer(const WeakPointer& other);
      WeakPointer(const SharedPointer& other);

      WeakPointer& operator=(WeakPointer<T> other);
      WeakPointer& operator=(SharedPointer<T> other);

      void reset();
      template<typename F> void reset(F *ptr);

      operator bool() const;

      unsigned int use_count() const;

    private:

      void swap(WeakPointer<T>& other);

	    unsigned int *m_referenceNumber;
	    T* m_ptr;
    };



    template<typename T> class WeakPointer<T[]>
    {
    public:

      WeakPointer();
	    WeakPointer(const WeakPointer& other);
      WeakPointer(const SharedPointer& other);

      WeakPointer& operator=(WeakPointer<T[]> other);
      WeakPointer& operator=(SharedPointer<T[]> other);

      void reset();
      template<typename F> void reset(F[] *ptr);

      operator bool() const;

      unsigned int use_count() const;

    private:

      void swap(WeakPointer<T[]>& other);

	    unsigned int *m_referenceNumber;
	    T* m_ptr;
    };
	}
}

#include "WeakPointer.inl"

#endif

#ifndef SHAREDPOINTER_HPP_
#define SHAREDPOINTER_HPP_

namespace he
{
	namespace util
	{
    template<typename T> class SharedPointer
    {
    public:

      SharedPointer()
      {
        m_obj = nullptr;
        m_referenceNumber = nullptr;
      }

	    SharedPointer(T* obj) : m_obj(obj), m_referenceNumber(new unsigned int(1))
	    {}

	    SharedPointer(const SharedPointer& other) : m_referenceNumber(other.m_referenceNumber), m_obj(other.m_obj)
	    {
        if(m_referenceNumber != nullptr)
        {
		      (*m_referenceNumber)++;
        }
	    }

	    ~SharedPointer()
	    {
		    if(m_referenceNumber != nullptr && --(*m_referenceNumber) == 0)
		    {
			    delete m_obj;
			    delete m_referenceNumber;
          m_referenceNumber = nullptr;
		    }
	    }

      SharedPointer& operator=(SharedPointer<T> other)
	    {
        if(*this == other)
        {
          return *this;
        }

        swap(other);

		    return *this;
	    }

      bool operator==(const SharedPointer& other) const
      {
        return m_referenceNumber == other.m_referenceNumber && m_obj == other.m_obj;
      }

      template<typename F> SharedPointer<F> cast() const
      {
        SharedPointer<F> ptr = SharedPointer<F>(dynamic_cast<F>(m_obj));
        ptr.m_referenceNumber = m_referenceNumber;
        (*m_referenceNumber)++;
        return ptr;
      }

      bool isNullptr() const
      {
        return m_obj == nullptr;
      }

	    T* operator->() const
	    {
		    return m_obj;
	    }

    private:

      void swap(SharedPointer<T>& other)
      {
        std::swap(m_referenceNumber, other.m_referenceNumber);
        std::swap(m_obj, other.m_obj);
      }

	    unsigned int *m_referenceNumber;
	    T* m_obj;
    };
	}
}

#endif

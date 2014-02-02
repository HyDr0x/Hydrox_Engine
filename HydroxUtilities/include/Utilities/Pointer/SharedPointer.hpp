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

	    SharedPointer(const SharedPointer& o) : m_referenceNumber(o.m_referenceNumber), m_obj(o.m_obj)
	    {
        if(m_referenceNumber != nullptr)
        {
		      (*m_referenceNumber)++;
        }
	    }

	    SharedPointer& operator=(const SharedPointer<T>& o)
	    {
		    if(o.m_obj == m_obj) 
        {
          return *this;
        }

		    if(m_referenceNumber != nullptr && --(*m_referenceNumber) == 0)
		    {
			    delete m_obj;
			    delete m_referenceNumber;
		    }

		    m_referenceNumber = o.m_referenceNumber;
		    m_obj = o.m_obj;

        if(m_referenceNumber != nullptr)
        {
		      (*m_referenceNumber)++;
        }

		    return *this;
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

      bool operator==(const SharedPointer& o)
      {
        return reinterpret_cast<unsigned int>(m_obj) == reinterpret_cast<unsigned int>(o.m_obj);
      }

      template<typename F> SharedPointer<F> cast()
      {
        SharedPointer<F> ptr = SharedPointer<F>(dynamic_cast<F>(m_obj));
        ptr.m_referenceNumber = m_referenceNumber;
        (*m_referenceNumber)++;
        return ptr;
      }

      bool isNullptr()
      {
        return m_obj == nullptr;
      }

	    T* operator->() const
	    {
		    return m_obj;
	    }

    private:

	    unsigned int *m_referenceNumber;
	    T* m_obj;
    };
	}
}

#endif

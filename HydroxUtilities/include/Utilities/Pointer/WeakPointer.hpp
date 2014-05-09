#ifndef WEAKPOINTER_HPP_
#define WEAKPOINTER_HPP_

namespace he
{
	namespace util
	{
    template<typename T> class WeakPointer
    {
    public:

      WeakPointer() : m_obj(nullptr), m_referenceNumber(nullptr)
      {}

	    WeakPointer(T* obj) : m_obj(obj)
	    {}

	    WeakPointer(const WeakPointer& o) : m_obj(o.m_obj)
	    {}

	    ~WeakPointer()
	    {}

      WeakPointer& operator=(const WeakPointer& o)
	    {
		    if(o.m_obj == m_obj) 
        {
          return *this;
        }

		    if(m_obj != nullptr)
		    {
			    delete m_obj;
		    }

		    m_obj = o.m_obj;

		    return *this;
	    }

      bool operator==(const WeakPointer& o) const
      {
        return reinterpret_cast<unsigned int>(m_obj) == reinterpret_cast<unsigned int>(o.m_obj);
      }

	    T* operator->() const
	    {
		    return m_obj;
	    }

    private:

	    T* m_obj;
    };
	}
}

#endif

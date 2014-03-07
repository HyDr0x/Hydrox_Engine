#include "Utilities/Miscellaneous/ResourceHandle.h"

namespace he
{
	namespace util
	{
    ResourceHandle::ResourceHandle()
    {
      m_referenceCounter = nullptr;
      m_id = ~0;
    }

    ResourceHandle::ResourceHandle(unsigned int id, unsigned int *referenceCounter)
    {
      m_referenceCounter = referenceCounter;
      (*m_referenceCounter)++;
      m_id = id;
    }

    ResourceHandle::ResourceHandle(const ResourceHandle& o) : m_referenceCounter(o.m_referenceCounter), m_id(o.m_id)
    {
      m_observer = o.m_observer;

      if(m_referenceCounter != nullptr)
      {
		    (*m_referenceCounter)++;
      }
    }

    ResourceHandle& ResourceHandle::operator=(const ResourceHandle& o)
    {
      if(*this == o)
      {
        return *this;
      }

      if(m_referenceCounter != nullptr && --(*m_referenceCounter) == 0)
      {
        notify(this);
      }

      m_referenceCounter = o.m_referenceCounter;
      m_id = o.m_id;
      m_observer = o.m_observer;

      if(m_referenceCounter != nullptr)
      {
		    (*m_referenceCounter)++;
      }

      return *this;
    }

    ResourceHandle::~ResourceHandle()
    {
      if(m_referenceCounter != nullptr && --(*m_referenceCounter) == 0)
      {
        notify(this);
        m_referenceCounter = nullptr;
      }
    }

    bool ResourceHandle::operator==(const ResourceHandle& o)
    {
      return m_id == o.m_id && m_referenceCounter == o.m_referenceCounter;
    }

    void ResourceHandle::free()
    {
      if(m_referenceCounter != nullptr && --(*m_referenceCounter) == 0)
      {
        notify(this);
      }

      m_referenceCounter = nullptr;
      m_id = ~0;
    }

    unsigned int ResourceHandle::getID() const
    {
      return m_id;
    }
	}
}
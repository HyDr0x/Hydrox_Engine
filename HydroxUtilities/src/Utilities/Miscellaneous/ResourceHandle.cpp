#include "Utilities/Miscellaneous/ResourceHandle.h"

namespace he
{
	namespace util
	{
    ResourceHandle::ResourceHandle() : m_id(~0), m_referenceCounter(nullptr)
    {
    }

    ResourceHandle::ResourceHandle(unsigned int id) : m_id(id), m_referenceCounter(new unsigned int)
    {
      *m_referenceCounter = 1;
    }

    ResourceHandle::ResourceHandle(const ResourceHandle& other) : Subject(other), m_id(other.m_id), m_referenceCounter(other.m_referenceCounter)
    {
      if(m_referenceCounter != nullptr)
      {
        (*m_referenceCounter)++;
      }
    }

    ResourceHandle::~ResourceHandle()
    {
      free();
    }

    ResourceHandle& ResourceHandle::operator=(ResourceHandle other)
    {
      if(*this == other)
      {
        return *this;
      }

      swap(other);

      return *this;
    }

    void ResourceHandle::swap(ResourceHandle& other)
    {
      std::swap(m_id, other.m_id);
      std::swap(m_referenceCounter, other.m_referenceCounter);
    }

    bool ResourceHandle::operator==(const ResourceHandle& o) const
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
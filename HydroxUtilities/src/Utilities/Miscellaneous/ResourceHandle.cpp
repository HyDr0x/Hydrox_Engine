#include "Utilities/Miscellaneous/ResourceHandle.h"

namespace he
{
  namespace util
  {
    ResourceHandle::ResourceHandle() : m_id(~0), m_referenceCounter(nullptr)
    {
    }

    ResourceHandle::ResourceHandle(unsigned int id, uint64_t hash) : m_id(id), m_referenceCounter(new unsigned int(1))
    {
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
      using namespace std;
      swap(other);

      return *this;
    }

    ResourceHandle::operator bool() const
    {
      return m_id != ~0 && m_referenceCounter != nullptr;
    }

    void ResourceHandle::swap(ResourceHandle& other)
    {
      Subject::swap(other);
      std::swap(m_id, other.m_id);
      std::swap(m_referenceCounter, other.m_referenceCounter);
    }

    bool ResourceHandle::operator==(const ResourceHandle& other) const
    {
      return m_id == other.m_id && m_referenceCounter == other.m_referenceCounter;
    }

    bool ResourceHandle::operator!=(const ResourceHandle& other) const
    {
      return !operator==(other);
    }

    void ResourceHandle::free()
    {
      if(m_referenceCounter != nullptr && --(*m_referenceCounter) == 0)
      {
        notify(*this);
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
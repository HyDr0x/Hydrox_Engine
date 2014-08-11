#include "DataBase/ManagedResource.h"

namespace he
{
  namespace db
  {
    ManagedResource::ManagedResource()
    {
    }

    ManagedResource::ManagedResource(const ManagedResource& other)
    {
      m_hash = other.m_hash;
    }

    ManagedResource::~ManagedResource()
    {
    }

    ManagedResource& ManagedResource::operator=(const ManagedResource& other)
    {
      m_hash = other.m_hash;

      return *this;
    }

    const uint64_t ManagedResource::getHash() const
    {
      return m_hash;
    }

  }
}
#include "DataBase/ManagedResource.h"

namespace he
{
  namespace db
  {
    ManagedResource::ManagedResource() :
      m_dirtyHash(true)
    {
    }

    ManagedResource::ManagedResource(const ManagedResource& other)
    {
      m_hash = other.m_hash;
      m_dirtyHash = other.m_dirtyHash;
    }

    ManagedResource::~ManagedResource()
    {
    }

    ManagedResource& ManagedResource::operator=(const ManagedResource& other)
    {
      m_hash = other.m_hash;
      m_dirtyHash = other.m_dirtyHash;

      return *this;
    }

    const uint64_t ManagedResource::getHash()
    {
      if(m_dirtyHash)
      {
        m_dirtyHash = false;

        updateHash();
      }

      return m_hash;
    }

  }
}
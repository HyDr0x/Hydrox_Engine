#include "Hydrox/Utility/Miscellaneous/CachedResource.h"

namespace he
{
  CachedResource::CachedResource()
  {
  }

  CachedResource::CachedResource(const CachedResource& o)
  {
    m_hash = o.m_hash;
  }

  CachedResource& CachedResource::operator=(const CachedResource& o)
  {
    m_hash = o.m_hash;

    return *this;
  }

  CachedResource::~CachedResource()
  {
  }

  const uint64_t CachedResource::getHash() const
  {
    return m_hash;
  }
}
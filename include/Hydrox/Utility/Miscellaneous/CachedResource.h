#ifndef CACHEDRESOURCE_H_
#define CACHEDRESOURCE_H_

#include "Hydrox/DLLExport.h"

#include "Hydrox/Utility/Miscellaneous/MurMurHash3.h"

namespace he
{
  class GRAPHICAPI CachedResource
  {
  public:

    CachedResource();
    CachedResource(const CachedResource& o);
    CachedResource& operator=(const CachedResource& o);
    virtual ~CachedResource() = 0;

    virtual void free() = 0;

    const uint64_t getHash() const;

  protected:

    uint64_t m_hash;
  };
}

#endif

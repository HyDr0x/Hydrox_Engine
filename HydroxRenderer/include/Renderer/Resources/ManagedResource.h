#ifndef CACHEDRESOURCE_H_
#define CACHEDRESOURCE_H_

#include "Utilities/DLLExport.h"

#include "Utilities/Miscellaneous/MurMurHash3.h"

namespace he
{
  namespace renderer
  {
    class GRAPHICAPI ManagedResource
    {
    public:

      ManagedResource();
      ManagedResource(const ManagedResource& other);
      virtual ~ManagedResource() = 0;

      ManagedResource& operator=(const ManagedResource& other);

      virtual void free() = 0;

      const uint64_t getHash() const;

    protected:

      uint64_t m_hash;
    };
  }
}

#endif

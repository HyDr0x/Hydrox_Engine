#ifndef CACHEDRESOURCE_H_
#define CACHEDRESOURCE_H_

#include "Utilities/DLLExport.h"

#include "Utilities/Miscellaneous/MurMurHash3.h"

namespace he
{
  namespace db
  {
    class GRAPHICAPI ManagedResource
    {
    public:

      ManagedResource();
      ManagedResource(const ManagedResource& other);
      virtual ~ManagedResource() = 0;

      ManagedResource& operator=(const ManagedResource& other);

      virtual void free() = 0;

      const uint64_t getHash();

    protected:

      virtual void updateHash() = 0;

      uint64_t m_hash;

      bool m_dirtyHash;//hash value needs to be updated
    };
  }
}

#endif

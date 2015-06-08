#ifndef RESOURCEHANDLE_H_
#define RESOURCEHANDLE_H_

#include "Utilities/DLLExport.h"

#include "Utilities/Observer/Subject.hpp"
#include "Utilities/Miscellaneous/MurMurHash3.h"

namespace he
{
  namespace util
  {
    class GRAPHICAPI ResourceHandle : public Subject<ResourceHandle&>
    {
    public:

      ResourceHandle();
      ResourceHandle(unsigned int id, uint64_t hash);
      ResourceHandle(const ResourceHandle& other);
      ~ResourceHandle();

      ResourceHandle& operator=(ResourceHandle other);

      operator bool() const;

      inline bool operator==(const ResourceHandle& other) const;
      inline bool operator!=(const ResourceHandle& other) const;

      void free();

      unsigned int getID() const;

      class Less
      {
      public:
        inline bool operator()(const util::ResourceHandle& o1, const util::ResourceHandle& o2) const
        {
          return o1.getID() < o2.getID();
        }
      };

    private:

      void swap(ResourceHandle& other);

      unsigned int m_id;
      unsigned int *m_referenceCounter;
    };
  }
}

#endif

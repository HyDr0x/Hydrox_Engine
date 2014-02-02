#ifndef RESOURCEHANDLE_H_
#define RESOURCEHANDLE_H_

#include "Utilities/DLLExport.h"

#include "Utilities/Observer/Subject.hpp"

namespace he
{
	namespace util
	{
    class GRAPHICAPI ResourceHandle : public Subject<ResourceHandle*>
    {
    public:

      ResourceHandle();
      ResourceHandle(unsigned int id, unsigned int *referenceCounter);
      ResourceHandle(const ResourceHandle& o);
      ResourceHandle& operator=(const ResourceHandle& o);
      ~ResourceHandle();

      inline bool operator==(const ResourceHandle& o);

      void free();

      unsigned int getID();

    private:

      unsigned int m_id;
      unsigned int *m_referenceCounter;
    };
	}
}

#endif

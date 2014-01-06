#ifndef RESOURCEHANDLE_H_
#define RESOURCEHANDLE_H_

#include "Hydrox/DLLExport.h"

#include "Hydrox/Utility/Observer/Subject.hpp"

namespace he
{
  class GRAPHICAPI ResourceHandle : public Subject<ResourceHandle*>
  {
  public:

    ResourceHandle();
    ResourceHandle(unsigned int id, unsigned int *referenceCounter);
    ResourceHandle(const ResourceHandle& o);
    ResourceHandle& operator=(const ResourceHandle& o);
    ~ResourceHandle();

    void free();

    unsigned int getID();

  private:

    unsigned int m_id;
    unsigned int *m_referenceCounter;
  };
}

#endif

#ifndef CACHEDRESOURCE_H_
#define CACHEDRESOURCE_H_

#include "Hydrox/DLLExport.h"

namespace he
{
  class GRAPHICAPI CachedResource
  {
  public:

    CachedResource();
    virtual ~CachedResource() = 0;

    virtual void free() = 0;

  protected:

  };
}

#endif

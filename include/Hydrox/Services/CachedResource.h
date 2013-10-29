#ifndef CACHEDRESOURCE_H_
#define CACHEDRESOURCE_H_

#include "Hydrox/DLLExport.h"

class GRAPHICAPI CachedResource
{
public:

  typedef unsigned int CacheIDType;

  CachedResource();
  virtual ~CachedResource();

  virtual void free() = 0;

protected:

};

typedef CachedResource::CacheIDType ResourceHandle;

#endif

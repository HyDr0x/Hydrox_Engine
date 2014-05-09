#ifndef RESOURCELOADER_H_
#define RESOURCELOADER_H_

#include <string>

#include "Loader/DLLExport.h"

#include <Renderer/Resources/ResourceManager.hpp>
#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Miscellaneous/SingletonManager.hpp>

namespace he
{
  namespace loader
  {
    class GRAPHICAPI ResourceLoader
    {
    public:

      ResourceLoader(util::SingletonManager *singletonManager);
      ResourceLoader(const ResourceLoader& o);

      virtual ~ResourceLoader() = 0;

      virtual util::ResourceHandle loadResource(std::string filename) = 0;

      virtual util::ResourceHandle getDefaultResource() const = 0;

    protected:

      util::SingletonManager *m_singletonManager;
    };
  }
}

#endif

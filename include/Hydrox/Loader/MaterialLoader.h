#ifndef MATERIALLOADER_H_
#define MATERIALLOADER_H_

#include <string>

#include "Hydrox/Singletons/CacheManager.hpp"
#include "Hydrox/Utility/Miscellaneous/ResourceHandle.h"

namespace he
{
  class MaterialLoader
  {
  public:

    MaterialLoader();
    ~MaterialLoader();

    ResourceHandle loadMaterial(std::string materialFilename);

  private:

  };
}

#endif
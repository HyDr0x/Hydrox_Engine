#include "Hydrox/Loader/MaterialLoader.h"

#include <fstream>
#include <iostream>

#include "Hydrox/Graphics/Material.h"

namespace he
{
  MaterialLoader::MaterialLoader()
  {
  }

  MaterialLoader::~MaterialLoader()
  {
  }

  ResourceHandle MaterialLoader::loadMaterial(std::string materialFilename)
  {
    return ResourceHandle(0, nullptr);
  }
}
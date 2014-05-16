#include "Loader/ResourceLoader.h"

#include <sstream>
#include <fstream>
#include <iostream>

#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <Renderer/Resources/Material.h>

#include "Loader/RenderShaderLoader.h"
#include "Loader/ILDevilLoader.h"

namespace he
{
  namespace loader
  {
    ResourceLoader::ResourceLoader(util::SingletonManager *singletonManager) : m_singletonManager(singletonManager)
    {
    }

    ResourceLoader::ResourceLoader(const ResourceLoader& o)
    {
      m_singletonManager = o.m_singletonManager;
    }

    ResourceLoader::~ResourceLoader()
    {
    }
  }
}
#include "Loader/ResourceLoader.h"

#include <sstream>
#include <fstream>
#include <iostream>

#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <DataBase/Material.h>

#include "Loader/RenderShaderLoader.h"
#include "Loader/ILDevilLoader.h"

namespace he
{
  namespace loader
  {
    ResourceLoader::ResourceLoader(util::SingletonManager *singletonManager) : m_singletonManager(singletonManager)
    {
    }

    ResourceLoader::ResourceLoader(const ResourceLoader& other)
    {
      m_singletonManager = other.m_singletonManager;
    }

    ResourceLoader::~ResourceLoader()
    {
    }
  }
}
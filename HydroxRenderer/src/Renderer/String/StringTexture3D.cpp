#include "Renderer\String\StringTexture3D.h"

#include <vector>

#include <Utilities\Miscellaneous\SingletonManager.hpp>
#include <Utilities\Math\Math.hpp>

#include "Renderer\Resources\ResourceManager.hpp"

namespace he
{
  namespace renderer
  {
    StringTexture3D::StringTexture3D(util::SingletonManager *singletonManager, std::string text)
    {
      ModelManager *modelManager = singletonManager->getService<ModelManager>();

      unsigned int letterNumber = text.size();

      //std::vector<util::Vector<float, 2>>;
    }

    StringTexture3D::~StringTexture3D()
    {

    }
  }
}
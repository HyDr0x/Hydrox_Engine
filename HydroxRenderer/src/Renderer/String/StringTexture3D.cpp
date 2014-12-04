#include "Renderer/String/StringTexture3D.h"

#include <vector>

#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Math/Math.hpp>
#include <Utilities/Pointer/SharedPointer.h>

#include <DataBase/ResourceManager.hpp>

namespace he
{
  namespace renderer
  {
    StringTexture3D::StringTexture3D(util::SingletonManager *singletonManager, std::string text)
    {
      util::SharedPointer<db::ModelManager> modelManager = singletonManager->getService<db::ModelManager>();

      unsigned int letterNumber = text.size();

      //std::vector<util::vec2f>;
    }

    StringTexture3D::~StringTexture3D()
    {

    }
  }
}
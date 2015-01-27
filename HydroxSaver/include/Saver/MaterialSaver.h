#ifndef MATERIALSAVER_H_
#define MATERIALSAVER_H_

#include <string>
#include <vector>

#include <DataBase/ResourceManager.hpp>
#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Math/Math.hpp>

#include "Saver/DLLExport.h"

namespace he
{
  namespace util
  {
    class SingletonManager;
  }

  namespace renderer
  {
    class Material;
  }

  namespace saver
  {
    class GRAPHICAPI MaterialSaver//could have wrong debug lines
    {
    public:

      enum MaterialFileKeywords
      {
        DEFAULT,
        DIFFUSESTRENGTH,
        SPECULARSTRENGTH,
        AMBIENTSTRENGTH,
        SPECULAREXPONENT,
        TEXTUREOFFSET,
        DIFFUSETEXTURE,
        NORMALMAP,
        SPECULARMAP,
        DISPLACEMENTMAP,
      };

      static void save(std::string path, std::string filename, const util::ResourceHandle materialHandle, util::SingletonManager *singletonManager);
    };
  }
}

#endif

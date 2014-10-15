#ifndef MESHSAVER_H_
#define MESHSAVER_H_

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
    class Mesh;
  }

  namespace saver
  {
    class GRAPHICAPI MeshSaver//could have wrong debug lines
    {
      struct MeshMetaData
      {
        util::vec3f bbMax;
        util::vec3f bbMin;
        GLuint primitiveType;
        GLuint primitiveCount;
        GLuint indexCount;
        GLuint vertexCount;
        GLuint vertexDeclaration;
        GLuint vertexStride;
        GLuint vboSize;
      };

    public:

      static void save(std::string path, std::string filename, const util::ResourceHandle meshHandle, util::SingletonManager *singletonManager);
    };
  }
}

#endif

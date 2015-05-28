#ifndef CUBEGENERATOR_H_
#define CUBEGENERATOR_H_

#include <vector>

#include "Utilities/DLLExport.h"

#include "Utilities/Math/Math.hpp"

namespace he
{
  namespace util
  {
    class GRAPHICAPI CubeGenerator
    {
    public:

      static void generateCube(std::vector<vec3f>& position, unsigned int levelOfDetail);
      static void generateCube(std::vector<vec3f>& position, std::vector<vec3f>& normal, unsigned int levelOfDetail);
      static void generateCube(std::vector<vec3f>& position, std::vector<unsigned int>& index, unsigned int levelOfDetail);
      static void generateCube(std::vector<vec3f>& position, std::vector<unsigned int>& index, std::vector<vec3f>& normal, unsigned int levelOfDetail);

    private:

      static void generateIndexedPositions(std::vector<vec3f>& position, std::vector<unsigned int>& index);
      static void generateNonIndexedPositions(std::vector<vec3f>& position);

      static void generateIndexedNormals(std::vector<vec3f>& normal, const std::vector<vec3f>& positions);
      static void generateNonIndexedNormals(std::vector<vec3f>& normal, const std::vector<vec3f>& positions);

      static void generateIndexedTextures(std::vector<vec3f>& texture);
      static void generateNonIndexedTextures(std::vector<vec3f>& texture);
    };
  }
}
#endif

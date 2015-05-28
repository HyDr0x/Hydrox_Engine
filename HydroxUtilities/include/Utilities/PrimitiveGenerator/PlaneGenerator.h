#ifndef PLANEGENERATOR_H_
#define PLANEGENERATOR_H_

#include <vector>

#include "Utilities/DLLExport.h"

#include "Utilities/Math/Math.hpp"

namespace he
{
  namespace util
  {
    class GRAPHICAPI PlaneGenerator
    {
    public:

      static void generatePlane(std::vector<vec3f>& position, unsigned int levelOfDetail);
      static void generatePlane(std::vector<vec3f>& position, std::vector<vec3f>& normal, unsigned int levelOfDetail);
      static void generatePlane(std::vector<vec3f>& position, std::vector<vec2f>& texture, unsigned int levelOfDetail);
      static void generatePlane(std::vector<vec3f>& position, std::vector<vec3f>& normal, std::vector<vec2f>& texture, unsigned int levelOfDetail);

      static void generatePlane(std::vector<vec3f>& position, std::vector<unsigned int>& index, unsigned int levelOfDetail);
      static void generatePlane(std::vector<vec3f>& position, std::vector<unsigned int>& index, std::vector<vec3f>& normal, unsigned int levelOfDetail);
      static void generatePlane(std::vector<vec3f>& position, std::vector<unsigned int>& index, std::vector<vec2f>& texture, unsigned int levelOfDetail);
      static void generatePlane(std::vector<vec3f>& position, std::vector<unsigned int>& index, std::vector<vec3f>& normal, std::vector<vec2f>& texture, unsigned int levelOfDetail);

    private:

      static void generateIndexedPositions(std::vector<vec3f>& position, std::vector<unsigned int>& index, unsigned int levelOfDetail);
      static void generateNonIndexedPositions(std::vector<vec3f>& position, unsigned int levelOfDetail);
      
      static void generateIndexedNormals(const std::vector<vec3f>& position, const std::vector<unsigned int>& index, std::vector<vec3f>& normal);
      static void generateNonIndexedNormals(const std::vector<vec3f>& position, std::vector<vec3f>& normal);

      static void generateTextures(const std::vector<vec3f>& position, std::vector<vec2f>& texture);
    };
  }
}
#endif

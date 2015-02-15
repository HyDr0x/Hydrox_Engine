#ifndef DISCGENERATOR_H_
#define DISCGENERATOR_H_

#include <vector>

#include "Utilities/DLLExport.h"

#include "Utilities/Math/Math.hpp"

namespace he
{
  namespace util
  {
    class GRAPHICAPI DiscGenerator
    {
    public:

      static void generateDisc(float radius, vec3f direction, std::vector<vec3f>& position, unsigned int vertexNumber);
      static void generateDisc(float radius, vec3f direction, std::vector<vec3f>& position, std::vector<vec3f>& normal, unsigned int vertexNumber);
      static void generateDisc(float radius, vec3f direction, std::vector<vec3f>& position, std::vector<unsigned int>& index, unsigned int vertexNumber);
      static void generateDisc(float radius, vec3f direction, std::vector<vec3f>& position, std::vector<unsigned int>& index, std::vector<vec3f>& normal, unsigned int vertexNumber);

    private:

      static void generateIndexedPositions(float radius, vec3f direction, std::vector<vec3f>& position, std::vector<unsigned int>& index, unsigned int vertexNumber);
      static void generateNonIndexedPositions(float radius, vec3f direction, std::vector<vec3f>& position, unsigned int vertexNumber);
      
      static void generateNormals(vec3f direction, std::vector<vec3f>& normal, unsigned int size);

      static void generateIndexedTextures(vec3f direction, std::vector<vec3f>& texture);
      static void generateNonIndexedTextures(vec3f direction, std::vector<vec3f>& texture);
    };
  }
}
#endif

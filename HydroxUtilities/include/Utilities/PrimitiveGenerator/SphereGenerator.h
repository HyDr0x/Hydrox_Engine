#ifndef SPHEREGENERATOR_H_
#define SPHEREGENERATOR_H_

#include <vector>

#include "Utilities/Math/Math.hpp"#

#include "Utilities/DLLExport.h"

namespace he
{
  namespace util
  {
    class GRAPHICAPI SphereGenerator
    {
    public:

      static void generateSphere(float radius, std::vector<vec3f>& position, unsigned int levelOfDetail);
      static void generateSphere(float radius, std::vector<vec3f>& position, std::vector<vec3f>& normal, unsigned int levelOfDetail);
      static void generateSphere(float radius, std::vector<vec3f>& position, std::vector<unsigned int>& index, unsigned int levelOfDetail);
      static void generateSphere(float radius, std::vector<vec3f>& position, std::vector<unsigned int>& index, std::vector<vec3f>& normal, unsigned int levelOfDetail);

    private:

      static void createTetrahedron(float radius, std::vector<vec3f>& positions, std::vector<vec3f>& normals);
      static void createTetrahedron(float radius, std::vector<vec3f>& positions, std::vector<vec3f>& normals, std::vector<unsigned int>& indices);
    };
  }
}

#endif

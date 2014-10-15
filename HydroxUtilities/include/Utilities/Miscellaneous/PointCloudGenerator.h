#ifndef POINTCLOUDGENERATOR_H_
#define POINTCLOUDGENERATOR_H_

#include <vector>

#include "Utilities/Math/Math.hpp"

namespace he
{
  namespace util
  {
    class PointCloudGenerator
    {
    public:

      static void generateCaches(std::vector<vec3f>& caches, float meanArea, const std::vector<vec3f>& positions, std::vector<unsigned int>& indices = std::vector<unsigned int>());

    private:

    };
  }
}

#endif
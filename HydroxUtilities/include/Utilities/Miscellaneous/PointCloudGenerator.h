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

      static void generateCaches(std::vector<util::Vector<float, 3>>& caches, const std::vector<util::Vector<float, 3>>& positions, std::vector<unsigned int>& indices = std::vector<unsigned int>());

    private:

    };
  }
}

#endif
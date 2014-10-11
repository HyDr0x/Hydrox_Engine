#include "Utilities/Miscellaneous/PointCloudGenerator.h"

namespace he
{
  namespace util
  {
    void PointCloudGenerator::generateCaches(std::vector<util::Vector<float, 3>>& caches, const std::vector<util::Vector<float, 3>>& positions, std::vector<unsigned int>& indices)
    {
      unsigned int triangleNumber = 0;

      if(indices.empty())
      {
        triangleNumber = positions.size() / 3;

        for(unsigned int i = 0; i < triangleNumber; i++)
        {

        }
      }
      else
      {
        triangleNumber = indices.size() / 3;
      }
    }
  }
}
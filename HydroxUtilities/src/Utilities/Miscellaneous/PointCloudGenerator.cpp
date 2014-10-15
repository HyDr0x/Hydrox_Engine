#include "Utilities/Miscellaneous/PointCloudGenerator.h"

#include <random>

namespace he
{
  namespace util
  {
    void PointCloudGenerator::generateCaches(std::vector<vec3f>& caches, float meanArea, const std::vector<vec3f>& positions, std::vector<unsigned int>& indices)
    {
      unsigned int triangleNumber = 0;

      if(indices.empty())
      {
        triangleNumber = positions.size() / 3;

        for(unsigned int i = 0; i < triangleNumber; i++)
        {
          vec3f a = positions[i + 1] - positions[i];
          vec3f b = positions[i + 2] - positions[i];

          float area = math::cross(a, b).length();

          float randArea = rand() / RAND_MAX;

          if(randArea >= area / meanArea)
          {
            float x1 = rand() / RAND_MAX;
            float x2 = rand() / RAND_MAX;

            if(x1 + x2 > 1.0f)
            {
              x1 = 1.0f - x1;
              x2 = 1.0f - x2;
            }

            vec3f cache = x1 * a + x2 * b;
          }
        }
      }
      else
      {
        triangleNumber = indices.size() / 3;
      }
    }
  }
}
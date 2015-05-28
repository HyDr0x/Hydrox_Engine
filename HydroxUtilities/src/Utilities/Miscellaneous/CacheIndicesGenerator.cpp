#include "Utilities/Miscellaneous/CacheIndicesGenerator.h"

#include <cassert>
#include <random>
#include <cmath>
#include <algorithm>

namespace he
{
  namespace util
  {
    CacheIndicesGenerator::CacheIndicesGenerator()
    {
    }

    void CacheIndicesGenerator::sortCacheIndices(std::vector<util::vec2f>& indices, util::vec2f newValue)
    {
      std::vector<util::vec2f> sortedIndices(indices.size());

      float highestWeight = 0.0f;
      float bestIndex = FLT_MAX;

      for(unsigned int i = 0; i < indices.size(); i++)
      {
        if(newValue[1] > indices[i][1])
        {
          bestIndex = i;
          break;
        }

        sortedIndices[i] = indices[i];
      }

      if(bestIndex == FLT_MAX)
      {
        return;
      }

      sortedIndices[bestIndex] = newValue;

      for(unsigned int i = bestIndex; i < indices.size() - 1; i++)
      {
        sortedIndices[i + 1] = indices[i];
      }

      indices = sortedIndices;
    }

    void CacheIndicesGenerator::generateCacheIndizes(const std::vector<util::vec3f>& positions, const std::vector<util::vec3f>& normals, const std::vector<util::Cache>& caches, std::vector<util::cacheIndexType>& cacheIndizes0, std::vector<util::cacheIndexType>& cacheIndizes1)
    {
      cacheIndizes0.resize(positions.size());
      cacheIndizes1.resize(positions.size());

      for(unsigned int i = 0; i < positions.size(); i++)
      {
        const unsigned int CACHEINDEXSIZE = 8;

        std::vector<util::vec2f> cacheIndices(CACHEINDEXSIZE, util::vec2f(INT32_MAX, 0.0f));

        for(unsigned int j = 0; j < caches.size(); j++)
        {
          float length = (positions[i] - caches[j].position).length();
          //if(length <= maxDistance)
          {
            float weight = util::vec3f::dot(normals[i], caches[j].normal) / (length * length + 0.0001f);//more weight is better

            if(weight > cacheIndices.back()[1] && util::vec3f::dot(normals[i], caches[j].normal) > 0.0f)
            {
              sortCacheIndices(cacheIndices, util::vec2f(float(j), weight));
            }
          }
        }

        for(unsigned int j = 0; j < CACHEINDEXSIZE; j++)//convert the indices into the shader accessable format (invalid: INT32_MAX --> 0, valid: x --> x + 1)
        {
          cacheIndices[j][0] = cacheIndices[j][0] == INT32_MAX ? 0 : cacheIndices[j][0] + 1;
        }

        cacheIndizes0[i] = util::cacheIndexType(cacheIndices[0][0], cacheIndices[1][0], cacheIndices[2][0], cacheIndices[3][0]);
        cacheIndizes1[i] = util::cacheIndexType(cacheIndices[4][0], cacheIndices[5][0], cacheIndices[6][0], cacheIndices[7][0]);
      }
    }
  }
}
#ifndef CACHEINDICESGENERATOR_H_
#define CACHEINDICESGENERATOR_H_

#include <vector>
#include <list>
#include <unordered_set>
#include <cstdint>

#include "Utilities/Math/Math.hpp"
#include "Utilities/Miscellaneous/CacheGenerator.h"

#include "Utilities/DLLExport.h"

namespace he
{
  namespace util
  {
    class GRAPHICAPI CacheIndicesGenerator
    {
    public:

      CacheIndicesGenerator();

      void generateCacheIndizes(const std::vector<util::vec3f>& positions, const std::vector<util::vec3f>& normals, const std::vector<util::Cache>& caches, std::vector<util::cacheIndexType>& cacheIndizes0, std::vector<util::cacheIndexType>& cacheIndizes1);
      
    private:

      void sortCacheIndices(std::vector<util::vec2f>& indices, util::vec2f newValue);

      static const unsigned int CACHEINDEXSIZE = 8;
    };
  }
}

#endif
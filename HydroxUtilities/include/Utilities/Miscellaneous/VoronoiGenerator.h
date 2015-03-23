#ifndef VORONOIGENERATOR_H_
#define VORONOIGENERATOR_H_

#include <vector>
#include <list>
#include <map>

#include "Utilities/Math/Math.hpp"

#include "Utilities/Miscellaneous/CacheGenerator.h"

#include "Utilities/DLLExport.h"

namespace he
{
  namespace util
  {
    class GRAPHICAPI VoronoiGenerator
    {
    public:

      void initialize(float errorRate, float maxDistance, float maxAngle);

      void generateCacheArea(const std::vector<vec3f>& triangles, std::vector<CacheGenerator::RawCache>& caches, vec3f bbMin, vec3f bbMax);
      
    private:

      static const int m_bitShiftNumber = 31;
    };
  }
}

#endif
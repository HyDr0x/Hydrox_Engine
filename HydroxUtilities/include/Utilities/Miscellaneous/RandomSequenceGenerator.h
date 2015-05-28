#ifndef RANDOMSEQUENCEGENERATOR_H_
#define RANDOMSEQUENCEGENERATOR_H_

#include <vector>
#include <list>
#include <map>

#include "Utilities/Math/Math.hpp"
#include "Utilities/Math/Polygon.h"
#include "Utilities/Math/Triangle.h"

#include "Utilities/DLLExport.h"

namespace he
{
  namespace util
  {
    class GRAPHICAPI RandomSequenceGenerator
    {
    public:

      static void generateDiscSequence(unsigned int sampleNumber, float radius, float distributionExponent, unsigned int debugTextureSize, std::vector<util::vec4f>& outSamplingPattern, std::vector<float>& outDebugSamplingPattern);

      static void generateHaltonSequence(unsigned int sampleNumber, util::vec2ui base, float offset, unsigned int debugTextureSize, std::vector<util::vec4f>& outSamplingPattern, std::vector<float>& outDebugSamplingPattern);

    private:

    };
  }
}

#endif
#include "Utilities/Miscellaneous/RandomSequenceGenerator.h"

#include <iostream>
#include <cassert>
#include <random>
#include <cmath>

namespace he
{
  namespace util
  {
    void RandomSequenceGenerator::generateDiscSequence(unsigned int sampleNumber, float radius, float distributionExponent, unsigned int debugTextureSize, std::vector<util::vec4f>& outSamplingPattern, std::vector<float>& outDebugSamplingPattern)
    {
      const float rMax = radius;

      outSamplingPattern.resize(sampleNumber);

      unsigned int imageSize = debugTextureSize;
      unsigned int imageSizeHalf = imageSize * 0.5f;
      float imageRaster = 1.0f / float(imageSize);
      float imageRasterHalf = 1.0f / float(2.0f * imageSize);
      outDebugSamplingPattern.resize(imageSize * imageSize, 0.0f);

      for(unsigned int i = 0; i < outSamplingPattern.size(); i++)
      {
        float s0 = rand() / float(RAND_MAX);
        float s1 = rand() / float(RAND_MAX);

        s0 = std::powf(s0, distributionExponent);

        outSamplingPattern[i] = util::vec2f(rMax * s0 * cosf(2.0f * util::math::PI * s1), rMax * s0 * sinf(2.0f * util::math::PI * s1));
        outSamplingPattern[i][2] = sqrt(s0);//std::powf(1.0f + s0 * radius, distributionExponent);
        //outSamplingPattern[i] *= s0 * s0;
        //outSamplingPattern[i] = outSamplingPattern[i].normalize();

        util::vec2f texCoords(unsigned int((outSamplingPattern[i][0] + rMax) / imageRaster) * imageRaster, unsigned int((outSamplingPattern[i][1] + rMax) / imageRaster) * imageRaster);
        int index = int(texCoords[0] * imageSize) + int(texCoords[1] * (imageSize - 1)) * imageSize;
        outDebugSamplingPattern[index] = outSamplingPattern[i][2];
      }
    }

    float RandomSequenceGenerator::halton(unsigned int index, unsigned int base)
    {
      unsigned int j = index;
      float f = 1.0f;
      float result = 0.0f;

      while(j > 0)
      {
        f /= base;
        result += f * (j % base);
        j /= base;
      }

      return result;
    }

    void RandomSequenceGenerator::generateHaltonSequence(unsigned int sampleNumber, util::vec2ui base, float offset, unsigned int debugTextureSize, std::vector<util::vec4f>& outSamplingPattern, std::vector<float>& outDebugSamplingPattern)
    {
      outSamplingPattern.resize(sampleNumber);

      unsigned int imageSize = debugTextureSize;
      unsigned int imageSizeHalf = imageSize * 0.5f;
      float imageRaster = 1.0f / float(imageSize);
      float imageRasterHalf = 1.0f / float(2.0f * imageSize);
      outDebugSamplingPattern.resize(imageSize * imageSize, 0.0f);

      for(unsigned int i = 0; i < outSamplingPattern.size(); i++)
      {
        outSamplingPattern[i][0] = halton(i + 1, base[0]) - offset;
        outSamplingPattern[i][1] = halton(i + 1, base[1]) - offset;
        outSamplingPattern[i][2] = 1.0f;

        util::vec2f texCoords(unsigned int((outSamplingPattern[i][0] + offset) / imageRaster) * imageRaster, unsigned int((outSamplingPattern[i][1] + offset) / imageRaster) * imageRaster);
        int index = int(texCoords[0] * imageSize) + int(texCoords[1] * (imageSize - 1)) * imageSize;
        outDebugSamplingPattern[index] = outSamplingPattern[i][2];
      }
    }
  }
}
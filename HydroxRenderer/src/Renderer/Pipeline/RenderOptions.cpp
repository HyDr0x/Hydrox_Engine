#include "Renderer/Pipeline/RenderOptions.h"

#include <string>

namespace he
{
  namespace renderer
  {
    void RenderOptions::loadConfigFile(std::string filename)
    {
      std::ifstream parameterStream;
      parameterStream.open(filename, std::ios::in);

      getParameter<unsigned int, int(*) (const char*)>(parameterStream, width, &atoi);
      getParameter<unsigned int, int(*) (const char*)>(parameterStream, height, &atoi);

      getParameter<unsigned int, int(*) (const char*)>(parameterStream, lightNumber, &atoi);
      getParameter<unsigned int, int(*) (const char*)>(parameterStream, shadowMapWidth, &atoi);

      getParameter<unsigned int, int(*) (const char*)>(parameterStream, reflectiveShadowMapWidth, &atoi);
      getParameter<unsigned int, int(*) (const char*)>(parameterStream, indirectShadowMapWidth, &atoi);
      getParameter<unsigned int, int(*) (const char*)>(parameterStream, giLightSampleNumber, &atoi);
      getParameter<unsigned int, int(*) (const char*)>(parameterStream, giShadowLightSampleDivisor, &atoi);
      getParameter<unsigned int, int(*) (const char*)>(parameterStream, kCache, &atoi);
      getParameter<unsigned int, int(*) (const char*)>(parameterStream, pushPullPyramideSize, &atoi);
      getParameter<unsigned int, int(*) (const char*)>(parameterStream, backProjectionWidth, &atoi);
      getParameter<unsigned int, int(*) (const char*)>(parameterStream, adaptiveSampleNumber, &atoi);

      getParameter<float, double(*) (const char*)>(parameterStream, paraboloidNearClipping, &atof);
      getParameter<float, double(*) (const char*)>(parameterStream, paraboloidFarClipping, &atof);

      getParameter<unsigned char, int(*) (const char*)>(parameterStream, max2DLayer, &atoi);

      getParameter<unsigned int, int(*) (const char*)>(parameterStream, vboBlockSize, &atoi);
      getParameter<unsigned int, int(*) (const char*)>(parameterStream, iboBlockSize, &atoi);
      triangleBlockSize = iboBlockSize / 3;

      getParameter<unsigned int, int(*) (const char*)>(parameterStream, cacheBlockSize, &atoi);
      getParameter<unsigned int, int(*) (const char*)>(parameterStream, perMeshBlockSize, &atoi);
      getParameter<unsigned int, int(*) (const char*)>(parameterStream, perInstanceBlockSize, &atoi);

      getParameter<unsigned int, int(*) (const char*)>(parameterStream, maxInstanceNumber, &atoi);
      getParameter<unsigned int, int(*) (const char*)>(parameterStream, maxMaterialNumber, &atoi);
      getParameter<unsigned int, int(*) (const char*)>(parameterStream, maxBoneNumber, &atoi);

      getParameter<unsigned int, int(*) (const char*)>(parameterStream, globalCacheBlockSize, &atoi);

      getParameter<unsigned int, int(*) (const char*)>(parameterStream, usedHistogramBins, &atoi);
      getParameter<float, double(*) (const char*)>(parameterStream, logLuminancePerMS, &atof);
      getParameter<float, double(*) (const char*)>(parameterStream, s, &atof);

      parameterStream.close();
    }

    void RenderOptions::overreadComments(std::ifstream &parameterStream, std::string & line)
    {
      bool eofbit;
      do
      {
        eofbit = std::getline(parameterStream, line).eof();

        if(line.size() != 0 && line[0] != '#')//if no comment
        {
          break;
        }
      } while(!eofbit);
    }
  }
}
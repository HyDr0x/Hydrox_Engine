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

      //Global Paths
      getParameter<std::string>(parameterStream, vfxPath);
      getParameter<std::string>(parameterStream, texturePath);
      getParameter<std::string>(parameterStream, modelPath);
      getParameter<std::string>(parameterStream, scenePath);

      //Framebuffer
      getParameter<unsigned int, int(*) (const char*)>(parameterStream, width, &atoi);
      getParameter<unsigned int, int(*) (const char*)>(parameterStream, height, &atoi);
      getParameter<unsigned int, int(*) (const char*)>(parameterStream, colorBitDepth, &atoi);
      getParameter<unsigned int, int(*) (const char*)>(parameterStream, depthBitDepth, &atoi);

      //Camera
      getParameter<float, double(*) (const char*)>(parameterStream, fov, &atof);
      getParameter<float, double(*) (const char*)>(parameterStream, nearPlane, &atof);
      getParameter<float, double(*) (const char*)>(parameterStream, farPlane, &atof);

      //Font
      getParameter<float, double(*) (const char*)>(parameterStream, letterSize, &atof);

      //Indirect Lighting
      getParameter<unsigned int, int(*) (const char*)>(parameterStream, vertexTileSize, &atoi);
      getParameter<unsigned int, int(*) (const char*)>(parameterStream, vertexTileMulti, &atoi);
      getParameter<unsigned int, int(*) (const char*)>(parameterStream, cacheTileSize, &atoi);
      getParameter<unsigned int, int(*) (const char*)>(parameterStream, indirectLightResolutionDivisor, &atoi);
      getParameter<unsigned int, int(*) (const char*)>(parameterStream, normalPhiQ, &atoi);
      getParameter<unsigned int, int(*) (const char*)>(parameterStream, normalThetaQ, &atoi);
      getParameter<unsigned int, int(*) (const char*)>(parameterStream, depthQ, &atoi);
      getParameter<float, double(*) (const char*)>(parameterStream, roughnessDiff, &atof);
      getParameter<unsigned int, int(*) (const char*)>(parameterStream, indexListGenerationCacheRounds, &atoi);
      
      //Lighting
      getParameter<unsigned int, int(*) (const char*)>(parameterStream, lightNumber, &atoi);
      getParameter<unsigned int, int(*) (const char*)>(parameterStream, shadowMapWidth, &atoi);

      //Shadows
      getParameter<unsigned int, int(*) (const char*)>(parameterStream, reflectiveShadowMapWidth, &atoi);
      getParameter<unsigned int, int(*) (const char*)>(parameterStream, indirectShadowMapWidth, &atoi);
      giLightSampleNumber = reflectiveShadowMapWidth * reflectiveShadowMapWidth;
      getParameter<unsigned int, int(*) (const char*)>(parameterStream, giShadowLightSampleDivisor, &atoi);
      getParameter<unsigned int, int(*) (const char*)>(parameterStream, indirectShadowMapTilenumber, &atoi);
      getParameter<unsigned int, int(*) (const char*)>(parameterStream, pushPullPyramideSize, &atoi);
      getParameter<unsigned int, int(*) (const char*)>(parameterStream, backProjectionWidth, &atoi);
      getParameter<unsigned int, int(*) (const char*)>(parameterStream, adaptiveSampleNumber, &atoi);
      getParameter<unsigned int, int(*) (const char*)>(parameterStream, specularCacheNumber, &atoi);

      getParameter<float, double(*) (const char*)>(parameterStream, paraboloidNearClipping, &atof);
      getParameter<float, double(*) (const char*)>(parameterStream, paraboloidFarClipping, &atof);

      //Sprites
      getParameter<unsigned char, int(*) (const char*)>(parameterStream, max2DLayer, &atoi);

      //Render Pipeline Buffer Sizes
      getParameter<unsigned int, int(*) (const char*)>(parameterStream, vboBlockSize, &atoi);
      getParameter<unsigned int, int(*) (const char*)>(parameterStream, iboBlockSize, &atoi);
      triangleBlockSize = iboBlockSize / 3;

      getParameter<unsigned int, int(*) (const char*)>(parameterStream, occluderBlockSize, &atoi);
      getParameter<unsigned int, int(*) (const char*)>(parameterStream, perMeshBlockSize, &atoi);
      getParameter<unsigned int, int(*) (const char*)>(parameterStream, perInstanceBlockSize, &atoi);

      getParameter<unsigned int, int(*) (const char*)>(parameterStream, maxInstanceNumber, &atoi);
      getParameter<unsigned int, int(*) (const char*)>(parameterStream, maxMaterialNumber, &atoi);
      getParameter<unsigned int, int(*) (const char*)>(parameterStream, maxBoneNumber, &atoi);

      getParameter<unsigned int, int(*) (const char*)>(parameterStream, globalOccluderBlockSize, &atoi);

      //Tonemapping
      getParameter<unsigned int, int(*) (const char*)>(parameterStream, usedHistogramBins, &atoi);
      getParameter<float, double(*) (const char*)>(parameterStream, logLuminancePerMS, &atof);
      getParameter<float, double(*) (const char*)>(parameterStream, s, &atof);

      //GI Options
      getParameter<bool, int(*) (const char*)>(parameterStream, globalIllumination, &atoi);
      getParameter<bool, int(*) (const char*)>(parameterStream, indirectShadows, &atoi);

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
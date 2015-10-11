#ifndef RENDEROPTIONS_H_
#define RENDEROPTIONS_H_

#include <fstream>

#include <Utilities/Miscellaneous/SingletonBehaviour.hpp>

#include "Renderer/DLLExport.h"

namespace he
{
  namespace renderer
  {
    class GRAPHICAPI RenderOptions : public util::SingletonBehaviour
    {
    public:

      ~RenderOptions(){}

      std::string vfxPath;
      std::string texturePath;
      std::string modelPath;
      std::string scenePath;

      unsigned int width;//horizontal resolution
      unsigned int height;//vertical resolution
      unsigned int colorBitDepth;//bit depth of the color buffer
      unsigned int depthBitDepth;//bit depth of the depth buffer

      float fov;//camera fov
      float nearPlane;//near clipping plane
      float farPlane;//far clipping plane

      float letterSize;//size of a single letter

      unsigned int shadowMapWidth;//resolution of the shadow maps
      unsigned int reflectiveShadowMapWidth;//resolution of the reflective shadow maps
      unsigned int giLightSampleNumber;//number of sampled lights for global illumination
      unsigned int giShadowLightSampleDivisor;//the divisor of the giLightSampleNumber variable: e.g. 4096 indirect light samples with shadowDivisor of 4 equals only 1024 indirect shadow light samples
      unsigned int kCache;//number of VAL's which are getting processed per invocation
      unsigned int lightNumber;//max number of supported lights

      unsigned int indirectShadowMapWidth;//resolution if the indirect shadow map
      unsigned int pushPullPyramideSize;//number of textures for the pushpull pyramide
      unsigned int backProjectionWidth;//size of the viewport for the backprojection, responsible for sample-hole detection and adaptive resampling of those regions to get a less erroneous ISM
      unsigned int adaptiveSampleNumber;//the maximum number of adaptive caches for hole-filling
      float paraboloidNearClipping;//near clipping plane for paraboloid projection
      float paraboloidFarClipping;//farclipping plane for paraboloid projection

      unsigned int specularCacheNumber;//maximum number of specular caches

      unsigned char max2DLayer;//const number of layer for 2D sprites

      //RenderNodes - const
      unsigned int maxMaterialNumber;//max number of materials in a material node
      unsigned int maxInstanceNumber;//max number of instances per render node
      unsigned int maxBoneNumber;//maximum number of bones per skinned mesh

      //RenderBuffer Block Sizes - const
      unsigned int vboBlockSize;
      unsigned int iboBlockSize;
      unsigned int triangleBlockSize;
      unsigned int cacheBlockSize;
      unsigned int perMeshBlockSize;
      unsigned int perInstanceBlockSize;

      unsigned int globalCacheBlockSize;//number of global cache block size

      //Tonemapping
      unsigned int usedHistogramBins;//tonemapping
      float logLuminancePerMS;//tonemapping
      float s;//tonemapping

      bool globalIllumination;//enables/disables global illumination
      bool indirectShadows;//enables/disables indirect shadows

      void loadConfigFile(std::string filename);

    private:

      void overreadComments(std::ifstream &parameterStream, std::string& line);

      template<typename TYPE, typename F> void getParameter(std::ifstream &parameterStream, TYPE &parameter, F cast)
      {
        std::string line;
        overreadComments(parameterStream, line);
        size_t pos = line.find(' ') + 1;
        parameter = cast(line.substr(pos).c_str());
      }

      template<typename TYPE> void getParameter(std::ifstream &parameterStream, TYPE &parameter)
      {
        std::string line;
        overreadComments(parameterStream, line);
        size_t pos = line.find(' ') + 1;
        parameter = line.substr(pos).c_str();
      }
    };
  }
}

#endif

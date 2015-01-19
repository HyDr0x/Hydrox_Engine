#ifndef RENDEROPTIONS_H_
#define RENDEROPTIONS_H_

#include <Utilities/Miscellaneous/SingletonBehaviour.hpp>

namespace he
{
  namespace renderer
  {
    class RenderOptions : public util::SingletonBehaviour
    {
    public:

      ~RenderOptions(){}

      unsigned int width;//horizontal resolution
      unsigned int height;//vertical resolution
      unsigned int shadowMapWidth;//resolution of the (reflective) shadow map
      unsigned int lightNumber;//max number of supported lights
      unsigned int unusedLightIndirectNumber;//the number of unused indirect lights of the reflected shadow map

      unsigned char max2DLayer;//const number of layer for 2D sprites

      //RenderNodes - const
      unsigned int maxMaterials;//max number of materials in a material node
      unsigned int maxGeometry;//max number of meshes per render node
      unsigned int maxBones;//maximum number of bones per skinned mesh

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
    };
  }
}

#endif

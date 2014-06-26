#ifndef RENDEROPTIONS_H_
#define RENDEROPTIONS_H_

namespace he
{
  namespace renderer
  {
    struct RenderOptions
    {
      unsigned int width;
      unsigned int height;
      unsigned int maxMaterials;
      unsigned int maxGeometry;
      unsigned int maxBones;
      unsigned int shadowMapWidth;
      unsigned int shadowMapHeight;
    };
  }
}

#endif

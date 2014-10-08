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

      unsigned int width;
      unsigned int height;
      unsigned int maxMaterials;
      unsigned int maxGeometry;
      unsigned int maxBones;
      unsigned int shadowMapWidth;
      unsigned int shadowMapHeight;
      unsigned int lightNumber;
      unsigned char max2DLayer;
    };
  }
}

#endif

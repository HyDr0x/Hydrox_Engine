#ifndef RENDERSHADERCONTAINER_H_
#define RENDERSHADERCONTAINER_H_

#include <string>

#include <Utilities/Miscellaneous/SingletonBehaviour.hpp>
#include <Utilities/Miscellaneous/ResourceHandle.h>

#include "Renderer/DLLExport.h"

namespace he
{
  namespace renderer
  {
    class GRAPHICAPI RenderShaderContainer : public util::SingletonBehaviour
    {
    public:

      ~RenderShaderContainer(){}

      he::util::ResourceHandle billboardShaderHandle;
      he::util::ResourceHandle spriteShaderHandle;
      he::util::ResourceHandle stringShaderHandle;
      he::util::ResourceHandle particleShaderHandle;
      he::util::ResourceHandle offscreenBufferShaderHandle;
      he::util::ResourceHandle directLightShaderHandle;
      he::util::ResourceHandle combineShaderHandle;
      he::util::ResourceHandle staticShadowMapGenerationShaderHandle;
      he::util::ResourceHandle skinnedShadowMapGenerationShaderHandle;
      he::util::ResourceHandle staticReflectiveShadowMapGenerationShaderHandle;
      he::util::ResourceHandle skinnedReflectiveShadowMapGenerationShaderHandle;
      he::util::ResourceHandle skyboxShaderHandle;
      he::util::ResourceHandle frustumCullingShaderHandle;
    };
  }
}

#endif

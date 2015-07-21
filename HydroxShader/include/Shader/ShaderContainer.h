#ifndef SHADERCONTAINER_H_
#define SHADERCONTAINER_H_

#include <string>

#include <Utilities/Miscellaneous/SingletonBehaviour.hpp>
#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Miscellaneous/Flags.hpp>

#include "Shader/RenderShader.h"
#include "Shader/ComputeShader.h"

#include "Shader/DLLExport.h"

namespace he
{
  namespace sh
  {
    class GRAPHICAPI ShaderContainer : public util::SingletonBehaviour
    {
    public:

      enum Renderpass
      {
        RENDERDEBUG = 0,
        GBUFFER = 1,
        INDIRECTSHADOWMAPCREATION = 2,
        VISIBILITYMAPCREATION = 3,
        TRIANGLEINDEX = 4,
        REFLECTIVESHADOW = 5,
        SHADOW = 6,
        INDIRECTLIGHTINTERPOLATION = 7,
        COMBINE = 8,
        DIRECTLIGHT = 9,
        DOWNSAMPLING = 10,
        OFFSCREENBUFFER = 11,
        TONEMAPPING = 12,
        HISTOGRAM = 13,
        BILLBOARD = 14,
        SKYBOX = 15,
        SPRITE = 16,
        STRING2D = 17,
        PUSHSHADER = 18,
        INDIRECTSHADOWMAPBACKPROJECTION = 19,
        JUMPFLOODALGORITHM = 20,
        PARABOLOIDCACHETOSITEPROJECTION = 21,
        SHOWVORONOIDIAGRAM = 22,
        PULLSHADER = 23,
        SEPARATEDBLUR = 24,
        PASSNUMBER
      };

      enum Computepass
      {
        FRUSTUMCULLING,
        INDIRECTLIGHTPROXYLIGHTCREATION,
        INDIRECTSHADOWMAPBLUR,
        PULL,
        INDIRECTSHADOWVIEWMATRIXCREATION,
        INDIRECTSHADOWPOINTSAMPLINGOFFSETCALCULATION,
        INDIRECTSHADOWPOINTSAMPLINGOFFSETSUMMATION,
        INDIRECTSHADOWPOINTSAMPLING,
        COMPUTESHADERNUMBER
      };

      struct ThisRenderShaderHandle
      {
        ThisRenderShaderHandle() : renderPass(Renderpass(UINT32_MAX)), shaderIndex(UINT32_MAX)
        {}

        ThisRenderShaderHandle(Renderpass pass, unsigned int index) : renderPass(pass), shaderIndex(index)
        {}

        operator bool()
        {
          return renderPass != Renderpass(UINT32_MAX) && shaderIndex != UINT32_MAX;
        }

        bool operator==(const ThisRenderShaderHandle& other) const
        {
          return renderPass == other.renderPass && shaderIndex == other.shaderIndex;
        }

        bool operator!=(const ThisRenderShaderHandle& other) const
        {
          return !(*this == other);
        }

        class Less
        {
        public:
          inline bool operator()(const ThisRenderShaderHandle& o1, const ThisRenderShaderHandle& o2) const
          {
            if(o1.renderPass == o2.renderPass)
            {
              return o1.shaderIndex < o2.shaderIndex;
            }
            else
            {
              return o1.renderPass < o2.renderPass;
            }
          }
        };

        Renderpass renderPass;
        unsigned int shaderIndex;
      };

      ShaderContainer();
      virtual ~ShaderContainer();

      ThisRenderShaderHandle getRenderShaderHandle(unsigned int pass, ShaderSlotFlags meshVertexDeclaration) const;
      std::vector<ThisRenderShaderHandle> getRenderShaderHandles(unsigned int pass) const;

      const RenderShader& getRenderShader(ThisRenderShaderHandle handle) const;
      const ComputeShader& getComputeShader(unsigned int shaderIndex) const;

      ThisRenderShaderHandle addRenderShader(unsigned int pass, const RenderShader& shader);
      void addComputeShader(unsigned int shaderIndex, const ComputeShader& shader);

      void replaceRenderShader(ThisRenderShaderHandle handle, const RenderShader& shader);
      void replaceComputeShader(unsigned int shaderIndex, const ComputeShader& shader);

    private:

      std::vector<std::vector<RenderShader>> m_renderPassShader;
      std::vector<ComputeShader> m_computeShader;
    };

    typedef ShaderContainer::ThisRenderShaderHandle RenderShaderHandle;
    typedef unsigned int ComputeShaderHandle;
  }
}

#endif

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

      enum RenderPass
      {
        RENDERDEBUG,
        GBUFFER,
        POINTINDEX,
        LINEINDEX,
        TRIANGLEINDEX,
        REFLECTIVESHADOW,
        SHADOW,
        INDIRECTLIGHTINTERPOLATION,
        COMBINE,
        DIRECTLIGHT,
        DOWNSAMPLING,
        INDIRECTLIGHTPROXYLIGHTCREATION,
        OFFSCREENBUFFER,
        TONEMAPPING,
        HISTOGRAM,
        BILLBOARD,
        SKYBOX,
        SPRITE,
        STRING2D,
        PASSNUMBER
      };

      enum ComputePass
      {
        FRUSTUMCULLING,
        COMPUTESHADERNUMBER
      };

      struct ThisRenderShaderHandle
      {
        ThisRenderShaderHandle() : renderPass(RenderPass(UINT32_MAX)), shaderIndex(UINT32_MAX)
        {}

        ThisRenderShaderHandle(RenderPass pass, unsigned int index) : renderPass(pass), shaderIndex(index)
        {}

        operator bool()
        {
          return renderPass != RenderPass(UINT32_MAX) && shaderIndex != UINT32_MAX;
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

        RenderPass renderPass;
        unsigned int shaderIndex;
      };

      ShaderContainer();
      virtual ~ShaderContainer();

      ThisRenderShaderHandle getRenderShaderHandle(unsigned int pass, ShaderSlotFlags meshVertexDeclaration) const;

      const RenderShader& getRenderShader(ThisRenderShaderHandle handle) const;
      const ComputeShader& getComputeShader(unsigned int shaderIndex) const;

      ThisRenderShaderHandle addRenderShader(unsigned int pass, const RenderShader& shader);
      void addComputeShader(unsigned int shaderIndex, const ComputeShader& shader);

      void replaceRenderShader(ThisRenderShaderHandle handle, const RenderShader& shader);

      unsigned int getRenderShaderPass(std::string completeFilename);
      unsigned int getComputeShaderIndex(std::string completeFilename);

    private:

      std::vector<std::vector<RenderShader>> m_renderPassShader;
      std::vector<ComputeShader> m_computeShader;

      std::map<std::string, unsigned int> m_renderShaderPass;
      std::map<std::string, unsigned int> m_computeShaderIndex;
    };

    typedef ShaderContainer::ThisRenderShaderHandle RenderShaderHandle;
  }
}

#endif

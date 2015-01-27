#ifndef SHADERCONTAINER_H_
#define SHADERCONTAINER_H_

#include <string>

#include <Utilities/Miscellaneous/SingletonBehaviour.hpp>
#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Miscellaneous/Flags.hpp>

#include "DataBase/Mesh.h"
#include "DataBase/ResourceManager.hpp"
#include "DataBase/DLLExport.h"

namespace he
{
  namespace db
  {
    class GRAPHICAPI ShaderContainer : public util::SingletonBehaviour
    {
    public:

      enum RenderPass
      {
        GBUFFER,
        INDEX,
        REFLECTIVESHADOW,
        SHADOW,
        COMBINE,
        DIRECTLIGHT,
        DOWNSAMPLING,
        INDIRECTLIGHTINTERPOLATION,
        OFFSCREENBUFFER,
        TONEMAPPING,
        HISTOGRAM,
        BILLBOARD,
        SKYBOX,
        SPRITE,
        STRING2D,
        PASSNUMBER
      };

      enum ComputeShader
      {
        FRUSTUMCULLING,
        INDIRECTLIGHT,
        COMPUTESHADERNUMBER
      };

      ShaderContainer()
      {
        m_renderPassShader.resize(PASSNUMBER);
        m_computeShader.resize(COMPUTESHADERNUMBER);
      }

      util::ResourceHandle getRenderShader(util::SingletonManager *singletonManager, unsigned int pass, util::Flags<db::VertexDeclarationFlags> meshVertexDeclaration) const
      {
        util::SharedPointer<db::RenderShaderManager> shaderManager = singletonManager->getService<db::RenderShaderManager>();

        util::ResourceHandle result;
        unsigned int maxBitNumber = 0;

        for(unsigned int i = 0; i < m_renderPassShader[pass].size(); i++)
        {
          util::Flags<db::VertexDeclarationFlags> shaderVertexDeclaration = shaderManager->getObject(m_renderPassShader[pass][i])->getVertexDeclaration();
          unsigned int bitNumber = (shaderVertexDeclaration & meshVertexDeclaration).raisedFlagNumber();
          unsigned int shaderBitNumber = shaderVertexDeclaration.raisedFlagNumber();

          if(bitNumber > maxBitNumber && bitNumber == shaderBitNumber)
          {
            maxBitNumber = bitNumber;
            result = m_renderPassShader[pass][i];
          }
        }

        return result;
      }

      util::ResourceHandle getComputeShader(util::SingletonManager *singletonManager, unsigned int shaderIndex) const
      {
        return m_computeShader[shaderIndex];
      }

      //he::util::ResourceHandle billboardShaderHandle;
      //he::util::ResourceHandle spriteShaderHandle;
      //he::util::ResourceHandle stringShaderHandle;
      //he::util::ResourceHandle particleShaderHandle;
      //he::util::ResourceHandle skyboxShaderHandle;
      //he::util::ResourceHandle indirectLightShaderHandle;
      //he::util::ResourceHandle indirectLightInterpolationShaderHandle;
      //he::util::ResourceHandle combineShaderHandle;
      //he::util::ResourceHandle offscreenBufferShaderHandle;
      //he::util::ResourceHandle directLightShaderHandle;

      //he::util::ResourceHandle frustumCullingShaderHandle;

      //he::util::ResourceHandle histogramShaderHandle;
      //he::util::ResourceHandle downSamplingShaderHandle;
      //he::util::ResourceHandle tonemappingShaderHandle;

      std::vector<std::vector<util::ResourceHandle>> m_renderPassShader;
      std::vector<util::ResourceHandle> m_computeShader;
    };
  }
}

#endif

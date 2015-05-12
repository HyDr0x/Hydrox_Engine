#include "Shader/ShaderContainer.h"

#include <float.h>

namespace he
{
  namespace sh
  {
    ShaderContainer::ShaderContainer()
    {
      m_renderPassShader.resize(PASSNUMBER);
      m_computeShader.resize(COMPUTESHADERNUMBER);
    }

    ShaderContainer::~ShaderContainer()
    {
    }

    ShaderContainer::ThisRenderShaderHandle ShaderContainer::getRenderShaderHandle(unsigned int pass, ShaderSlotFlags meshVertexDeclaration) const
    {
      unsigned int resultID;
      unsigned int maxBitNumber = 0;

      for(unsigned int i = 0; i < m_renderPassShader[pass].size(); i++)
      {
        ShaderSlotFlags shaderVertexDeclaration = m_renderPassShader[pass][i].getVertexDeclaration();
        unsigned int bitNumber = (shaderVertexDeclaration & meshVertexDeclaration).raisedFlagNumber();
        unsigned int shaderBitNumber = shaderVertexDeclaration.raisedFlagNumber();

        if(bitNumber > maxBitNumber && bitNumber == shaderBitNumber)
        {
          maxBitNumber = bitNumber;
          resultID = i;
        }
      }

      return ThisRenderShaderHandle(RenderPass(pass), resultID);
    }

    const RenderShader& ShaderContainer::getRenderShader(ThisRenderShaderHandle handle) const
    {
      return m_renderPassShader[handle.renderPass][handle.shaderIndex];
    }

    const ComputeShader& ShaderContainer::getComputeShader(unsigned int shaderIndex) const
    {
      return m_computeShader[shaderIndex];
    }

    sh::ShaderContainer::ThisRenderShaderHandle ShaderContainer::addRenderShader(unsigned int pass, const RenderShader& shader)
    {
      m_renderPassShader[pass].push_back(shader);
      return ThisRenderShaderHandle(RenderPass(pass), m_renderPassShader[pass].size() - 1);
    }

    void ShaderContainer::addComputeShader(unsigned int shaderIndex, const ComputeShader& shader)
    {
      m_computeShader[shaderIndex] = shader;
    }

    void ShaderContainer::replaceRenderShader(ThisRenderShaderHandle handle, const RenderShader& shader)
    {
      m_renderPassShader[handle.renderPass][handle.shaderIndex] = shader;
    }

    unsigned int ShaderContainer::getRenderShaderPass(std::string completeFilename)
    {
      return m_renderShaderPass[completeFilename];
    }

    unsigned int ShaderContainer::getComputeShaderIndex(std::string completeFilename)
    {
      return m_computeShaderIndex[completeFilename];
    }
  }
}
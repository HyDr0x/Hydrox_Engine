#include "Shader/ShaderContainer.h"

#include <float.h>

namespace he
{
  namespace sh
  {
    ShaderContainer::ShaderContainer()
    {
      m_computeShader.resize(COMPUTESHADERNUMBER);
      m_renderPassShader.resize(PASSNUMBER);
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

      return ThisRenderShaderHandle(Renderpass(pass), resultID);
    }

    std::vector<ShaderContainer::ThisRenderShaderHandle> ShaderContainer::getRenderShaderHandles(unsigned int pass) const
    {
      std::vector<ThisRenderShaderHandle> renderShaderHandles(m_renderPassShader[pass].size());

      for(unsigned int i = 0; i < m_renderPassShader[pass].size(); i++)
      {
        renderShaderHandles[i] = ThisRenderShaderHandle(Renderpass(pass), i);
      }

      return renderShaderHandles;
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
      return ThisRenderShaderHandle(Renderpass(pass), m_renderPassShader[pass].size() - 1);
    }

    void ShaderContainer::addComputeShader(unsigned int shaderIndex, const ComputeShader& shader)
    {
      m_computeShader[shaderIndex] = shader;
    }

    void ShaderContainer::replaceRenderShader(ThisRenderShaderHandle handle, const RenderShader& shader)
    {
      m_renderPassShader[handle.renderPass][handle.shaderIndex] = shader;
    }

    void ShaderContainer::replaceComputeShader(unsigned int shaderIndex, const ComputeShader& shader)
    {
      m_computeShader[shaderIndex] = shader;
    }
  }
}
#include "Renderer/Renderpass/ARenderpass.h"

#include <DataBase/Mesh.h>

#include "Renderer/Pipeline/RenderOptions.h"

namespace he
{
  namespace renderer
  {
    ARenderpass::ARenderpass()
    {
      
    }

    ARenderpass::~ARenderpass()
    {
    }

    void ARenderpass::initialize(util::SingletonManager *singletonManager, sh::ShaderContainer::Renderpass renderPass)
    {
      m_options = singletonManager->getService<RenderOptions>();
      m_renderPass = renderPass;
      m_shaderContainer = singletonManager->getService<sh::ShaderContainer>();

      std::vector<sh::RenderShaderHandle> renderShaderHandles = m_shaderContainer->getRenderShaderHandles(renderPass);

      m_shaderNodes.resize(renderShaderHandles.size());
      m_vertexDeclarationNodes.resize(renderShaderHandles.size());
      m_textureNodes.resize(renderShaderHandles.size());
      m_renderContainer.resize(renderShaderHandles.size());

      for(unsigned int i = 0; i < m_shaderNodes.size(); i++)
      {
        m_shaderNodes[i].initialize(renderShaderHandles[i]);
      }
    }

    bool ARenderpass::insertGeometry(util::SharedPointer<IRenderContainer> renderContainer)
    {
      sh::RenderShaderHandle shaderHandle = m_shaderContainer->getRenderShaderHandle(m_renderPass, sh::ShaderSlotFlags(renderContainer->getMeshVertexDeclaration().toInt()));

      if(shaderHandle.shaderIndex != ~0)
      {
        const sh::RenderShader& shader = m_shaderContainer->getRenderShader(shaderHandle);

        for(unsigned int i = 0; i < m_shaderNodes.size(); i++)
        {
          if(m_shaderNodes[i].isShader(shaderHandle))
          {
            VertexDeclarationNode vertexDeclarationNode;
            vertexDeclarationNode.initialize(shader.getVertexDeclaration(), renderContainer->getMeshVertexDeclaration());

            m_vertexDeclarationNodes[i].resize(m_vertexDeclarationNodes[i].size() + 1);//necessary because no const ctor possible, because of unique pointer
            m_vertexDeclarationNodes[i][m_vertexDeclarationNodes[i].size() - 1] = vertexDeclarationNode;

            TextureNode textureNode;
            textureNode.initialize(renderContainer->getTextureHandles());
            m_textureNodes[i].push_back(textureNode);

            m_renderContainer[i].push_back(renderContainer);

            return true;
          }
        }
      }

      return false;
    }

    bool ARenderpass::removeGeometry(util::SharedPointer<IRenderContainer> renderContainer)
    {
      sh::RenderShaderHandle shaderHandle = m_shaderContainer->getRenderShaderHandle(m_renderPass, sh::ShaderSlotFlags(renderContainer->getMeshVertexDeclaration().toInt()));

      if(shaderHandle)
      {
        const sh::RenderShader& shader = m_shaderContainer->getRenderShader(shaderHandle);

        for(unsigned int i = 0; i < m_shaderNodes.size(); i++)
        {
          if(m_shaderNodes[i].isShader(shaderHandle))
          {
            for(unsigned int j = 0; j < m_vertexDeclarationNodes[i].size(); j++)
            {
              if(m_renderContainer[i][j] == renderContainer)
              {
                m_vertexDeclarationNodes[i].erase(m_vertexDeclarationNodes[i].begin() + j);
                m_textureNodes[i].erase(m_textureNodes[i].begin() + j);
                m_renderContainer[i].erase(m_renderContainer[i].begin() + j);

                return true;
              }
            }
          }
        }
      }

      return false;
    }
  }
}

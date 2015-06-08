#include "Renderer/Renderpass/ReflectiveShadowMapGeometryRenderpass.h"

#include <DataBase/Mesh.h>

#include "Renderer/Pipeline/RenderOptions.h"

namespace he
{
  namespace renderer
  {
    ReflectiveShadowMapGeometryRenderpass::ReflectiveShadowMapGeometryRenderpass()
    {
    }

    ReflectiveShadowMapGeometryRenderpass::~ReflectiveShadowMapGeometryRenderpass()
    {
    }

    void ReflectiveShadowMapGeometryRenderpass::initialize(util::SingletonManager *singletonManager, sh::ShaderContainer::Renderpass renderPass, std::vector<util::SharedPointer<SamplerObject>> samplerObjects)
    {
      ARenderpass::initialize(singletonManager, renderPass);

      m_textureManager = singletonManager->getService<db::TextureManager>();
      m_samplerObjects = samplerObjects;
    }

    void ReflectiveShadowMapGeometryRenderpass::setLightIndex(int lightIndex)
    {
      m_lightIndex = lightIndex;
    }

    void ReflectiveShadowMapGeometryRenderpass::drawRenderpass() const
    {
      for(unsigned int i = 0; i < m_shaderNodes.size(); i++)
      {
        const sh::RenderShader& shader = m_shaderContainer->getRenderShader(m_shaderNodes[i].getShaderHandle());
        shader.useShader();

        sh::RenderShader::setUniform(2, GL_INT, &m_lightIndex);
        sh::RenderShader::setUniform(3, GL_INT, &m_options->shadowMapWidth);

        for(unsigned int j = 0; j < m_renderContainer[i].size(); j++)//all arrays have the same size! Vertexdeclaration-, Texture Nodes and Render Container are coupled together
        {
          m_vertexDeclarationNodes[i][j].setVertexArray();

          m_textureNodes[i][j].bindTextures(m_textureManager, m_samplerObjects);

          m_renderContainer[i][j]->getMaterialData().bindMaterialBuffer();
          m_renderContainer[i][j]->getMatrixData().bindMatrixBuffer();
          m_renderContainer[i][j]->getDrawData().bindDrawBuffer();
          m_renderContainer[i][j]->getDrawData().draw();
          m_renderContainer[i][j]->getDrawData().unbindDrawBuffer();
          m_renderContainer[i][j]->getMatrixData().unbindMatrixBuffer();
          m_renderContainer[i][j]->getMaterialData().unbindMaterialBuffer();

          m_textureNodes[i][j].unbindTextures(m_textureManager, m_samplerObjects);
          
          m_vertexDeclarationNodes[i][j].unsetVertexArray();
        }

        shader.useNoShader();
      }
    }
  }
}

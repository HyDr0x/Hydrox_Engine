#include "Renderer/Renderpass/IndirectLightGeometryRenderpass.h"

#include <DataBase/Mesh.h>

#include "Renderer/Pipeline/RenderOptions.h"

namespace he
{
  namespace renderer
  {
    IndirectLightGeometryRenderpass::IndirectLightGeometryRenderpass()
    {
    }

    IndirectLightGeometryRenderpass::~IndirectLightGeometryRenderpass()
    {
    }

    void IndirectLightGeometryRenderpass::initialize(util::SingletonManager *singletonManager, sh::ShaderContainer::Renderpass renderPass,
      util::SharedPointer<db::Texture2D> normalMap, util::SharedPointer<db::Texture2D> materialMap
      )
    {
      ARenderpass::initialize(singletonManager, renderPass);

      m_normalMap = normalMap;
      m_materialMap = materialMap;
    }

    void IndirectLightGeometryRenderpass::setProxyLightTextureResolution(GLuint proxyLightTextureResolution)
    {
      m_proxyLightTextureResolution = proxyLightTextureResolution;
    }

    void IndirectLightGeometryRenderpass::setIndirectShadowMap(util::SharedPointer<db::Texture2D> indirectShadowMap)
    {
      m_indirectShadowMap = indirectShadowMap;
    }

    void IndirectLightGeometryRenderpass::drawRenderpass() const
    {
      GLuint globalCacheNumber = 0;

      for(unsigned int i = 0; i < m_shaderNodes.size(); i++)
      {
        const sh::RenderShader& shader = m_shaderContainer->getRenderShader(m_shaderNodes[i].getShaderHandle());
        shader.useShader();

        sh::RenderShader::setUniform(3, GL_UNSIGNED_INT, &m_proxyLightTextureResolution);

        m_normalMap->setTexture(0, 0);
        m_materialMap->setTexture(1, 1);
        m_indirectShadowMap->setTexture(2, 2);

        for(unsigned int j = 0; j < m_renderContainer[i].size(); j++)//all arrays have the same size! Vertexdeclaration-, Texture Nodes and Render Container are coupled together
        {
          sh::RenderShader::setUniform(4, GL_UNSIGNED_INT, &globalCacheNumber);

          m_vertexDeclarationNodes[i][j].setVertexArray();

          m_renderContainer[i][j]->getMatrixData().bindMatrixBuffer();
          m_renderContainer[i][j]->getDrawData().bindDrawBuffer();
          m_renderContainer[i][j]->getDrawData().bindCacheInstanceOffsetBuffer();
          m_renderContainer[i][j]->getDrawData().draw();
          m_renderContainer[i][j]->getDrawData().unbindCacheInstanceOffsetBuffer();
          m_renderContainer[i][j]->getDrawData().unbindDrawBuffer();
          m_renderContainer[i][j]->getMatrixData().unbindMatrixBuffer();

          m_vertexDeclarationNodes[i][j].unsetVertexArray();

          globalCacheNumber += m_renderContainer[i][j]->getCacheNumber();
        }

        m_indirectShadowMap->unsetTexture(2);
        m_materialMap->unsetTexture(1);
        m_normalMap->unsetTexture(0);

        shader.useNoShader();
      }
    }
  }
}

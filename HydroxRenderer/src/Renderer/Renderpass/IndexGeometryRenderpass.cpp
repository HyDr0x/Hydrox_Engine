#include "Renderer/Renderpass/IndexGeometryRenderpass.h"

#include <DataBase/Mesh.h>

#include "Renderer/Pipeline/RenderOptions.h"

namespace he
{
  namespace renderer
  {
    IndexGeometryRenderpass::IndexGeometryRenderpass()
    {
    }

    IndexGeometryRenderpass::~IndexGeometryRenderpass()
    {
    }

    void IndexGeometryRenderpass::setProxyLightTextureResolution(GLuint proxyLightTextureResolution)
    {
      m_proxyLightTextureResolution = proxyLightTextureResolution;
    }

    void IndexGeometryRenderpass::drawRenderpass() const
    {
      GLuint globalCacheNumber = 0;

      for(unsigned int i = 0; i < m_shaderNodes.size(); i++)
      {
        const sh::RenderShader& shader = m_shaderContainer->getRenderShader(m_shaderNodes[i].getShaderHandle());
        shader.useShader();

        sh::RenderShader::setUniform(1, GL_UNSIGNED_INT, &m_proxyLightTextureResolution);

        for(unsigned int j = 0; j < m_renderContainer[i].size(); j++)//all arrays have the same size! Vertexdeclaration-, Texture Nodes and Render Container are coupled together
        {
          sh::RenderShader::setUniform(0, GL_UNSIGNED_INT, &globalCacheNumber);

          m_vertexDeclarationNodes[i][j].setVertexArray();

          m_renderContainer[i][j]->getMaterialData().bindMaterialBuffer();
          m_renderContainer[i][j]->getMatrixData().bindMatrixBuffer();
          m_renderContainer[i][j]->getDrawData().bindDrawBuffer();
          m_renderContainer[i][j]->getDrawData().bindCacheBuffer();
          m_renderContainer[i][j]->getDrawData().draw();
          m_renderContainer[i][j]->getDrawData().unbindCacheBuffer();
          m_renderContainer[i][j]->getDrawData().unbindDrawBuffer();
          m_renderContainer[i][j]->getMatrixData().unbindMatrixBuffer();
          m_renderContainer[i][j]->getMaterialData().unbindMaterialBuffer();

          m_vertexDeclarationNodes[i][j].unsetVertexArray();

          globalCacheNumber += m_renderContainer[i][j]->getPerInstanceCacheNumber();
        }

        shader.useNoShader();
      }
    }
  }
}

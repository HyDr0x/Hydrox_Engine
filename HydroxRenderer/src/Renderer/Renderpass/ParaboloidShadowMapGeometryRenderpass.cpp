#include "Renderer/Renderpass/ParaboloidShadowMapGeometryRenderpass.h"

#include <DataBase/Mesh.h>

#include "Renderer/Pipeline/RenderOptions.h"

namespace he
{
  namespace renderer
  {
    ParaboloidShadowMapGeometryRenderpass::ParaboloidShadowMapGeometryRenderpass() : m_lightIndex(0)
    {
    }

    ParaboloidShadowMapGeometryRenderpass::~ParaboloidShadowMapGeometryRenderpass()
    {
    }

    void ParaboloidShadowMapGeometryRenderpass::setLightIndex(int lightIndex)
    {
      m_lightIndex = lightIndex;
    }

    void ParaboloidShadowMapGeometryRenderpass::drawRenderpass() const
    {
      for(unsigned int i = 0; i < m_shaderNodes.size(); i++)
      {
        const sh::RenderShader& shader = m_shaderContainer->getRenderShader(m_shaderNodes[i].getShaderHandle());
        shader.useShader();

        sh::RenderShader::setUniform(2, GL_INT, &m_lightIndex);

        for(unsigned int j = 0; j < m_renderContainer[i].size(); j++)//all arrays have the same size! Vertexdeclaration-, Texture Nodes and Render Container are coupled together
        {
          m_vertexDeclarationNodes[i][j].setVertexArray();

          m_renderContainer[i][j]->getMatrixData().bindMatrixBuffer();
          m_renderContainer[i][j]->getDrawData().bindDrawBuffer();
          m_renderContainer[i][j]->getDrawData().draw();
          m_renderContainer[i][j]->getDrawData().unbindDrawBuffer();
          m_renderContainer[i][j]->getMatrixData().unbindMatrixBuffer();

          m_vertexDeclarationNodes[i][j].unsetVertexArray();
        }

        shader.useNoShader();
      }
    }
  }
}

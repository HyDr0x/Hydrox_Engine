#include "Renderer/Renderpass/IndirectBackprojectionRenderpass.h"

#include <DataBase/Mesh.h>

#include "Renderer/Pipeline/RenderOptions.h"

namespace he
{
  namespace renderer
  {
    IndirectBackprojectionRenderpass::IndirectBackprojectionRenderpass() : m_lightIndex(0)
    {
    }

    IndirectBackprojectionRenderpass::~IndirectBackprojectionRenderpass()
    {
    }

    bool IndirectBackprojectionRenderpass::insertGeometry(util::SharedPointer<IRenderContainer> renderContainer)
    {
      if(renderContainer->getDrawData().getPrimitiveType() == GL_TRIANGLES)
      {
        return ARenderpass::insertGeometry(renderContainer);
      }

      return false;
    }

    void IndirectBackprojectionRenderpass::setLightIndex(int lightIndex)
    {
      m_lightIndex = lightIndex;
    }

    void IndirectBackprojectionRenderpass::drawRenderpass() const
    {
      for(unsigned int i = 0; i < m_shaderNodes.size(); i++)
      {
        const sh::RenderShader& shader = m_shaderContainer->getRenderShader(m_shaderNodes[i].getShaderHandle());
        shader.useShader();

        sh::RenderShader::setUniform(0, GL_UNSIGNED_INT, &m_lightIndex);

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

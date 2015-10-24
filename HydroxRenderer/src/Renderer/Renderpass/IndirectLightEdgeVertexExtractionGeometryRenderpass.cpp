#include "Renderer/Renderpass/IndirectLightEdgeVertexExtractionGeometryRenderpass.h"

#include <DataBase/Mesh.h>

#include "Renderer/Pipeline/RenderOptions.h"

namespace he
{
  namespace renderer
  {
    IndirectLightEdgeVertexExtractionGeometryRenderpass::IndirectLightEdgeVertexExtractionGeometryRenderpass()
    {
    }

    IndirectLightEdgeVertexExtractionGeometryRenderpass::~IndirectLightEdgeVertexExtractionGeometryRenderpass()
    {
    }

    void IndirectLightEdgeVertexExtractionGeometryRenderpass::initialize(util::SingletonManager *singletonManager, sh::ShaderContainer::Renderpass renderPass, util::SharedPointer<db::Texture2D> gBufferMaterialMap)
    {
      m_gBufferMaterialMap = gBufferMaterialMap;
      ARenderpass::initialize(singletonManager, renderPass);
    }

    void IndirectLightEdgeVertexExtractionGeometryRenderpass::drawRenderpass() const
    {
      for(unsigned int i = 0; i < m_shaderNodes.size(); i++)
      {
        const sh::RenderShader& shader = m_shaderContainer->getRenderShader(m_shaderNodes[i].getShaderHandle());
        shader.useShader();

        m_gBufferMaterialMap->setTexture(0, 0);

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

        m_gBufferMaterialMap->unsetTexture(0);

        shader.useNoShader();
      }
    }
  }
}

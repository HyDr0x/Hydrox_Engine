#include "Renderer/Renderpass/FrustumCullingRenderpass.h"

#include <DataBase/Mesh.h>

#include "Renderer/Pipeline/RenderOptions.h"

namespace he
{
  namespace renderer
  {
    FrustumCullingRenderpass::FrustumCullingRenderpass() : m_viewIndex(UINT32_MAX)
    {
    }

    FrustumCullingRenderpass::~FrustumCullingRenderpass()
    {
    }

    void FrustumCullingRenderpass::setViewIndex(int viewIndex)
    {
      m_viewIndex = viewIndex;
    }

    void FrustumCullingRenderpass::doComputepass() const
    {
      const sh::ComputeShader& shader = m_shaderContainer->getComputeShader(m_renderPass);
      shader.useShader();

      sh::ComputeShader::setUniform(1, GL_INT, &m_viewIndex);

      for(unsigned int i = 0; i < m_renderContainer.size(); i++)
      {
        m_renderContainer[i]->getDrawData().bindFrustumCullingBuffer();
        m_renderContainer[i]->getMatrixData().bindMatrixBuffer();
        sh::ComputeShader::dispatchComputeShader(128, 1, 1);
        m_renderContainer[i]->getMatrixData().unbindMatrixBuffer();
        m_renderContainer[i]->getDrawData().unbindFrustumCullingBuffer();
      }

      shader.useNoShader();
    }
  }
}

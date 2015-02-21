#include "Renderer/TreeNodes/RenderNodeDecorator/StaticGeometryDecorator.h"

#include <XBar/StaticGeometryContainer.h>

#include "Renderer/Traverser/Traverser.h"

namespace he
{
  namespace renderer
  {
    StaticGeometryDecorator::StaticGeometryDecorator(IRenderGroup *renderNode, util::SharedPointer<RenderOptions> options) : ARenderNodeDecorator(renderNode), m_options(options)
    {
      m_matrixBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(util::Matrix<float, 4>) * m_options->perInstanceBlockSize, 0, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT, nullptr);
    }

    StaticGeometryDecorator::~StaticGeometryDecorator()
    {
    }

    void StaticGeometryDecorator::frustumCulling() const
    {
      m_matrixBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      m_renderNode->frustumCulling();

      m_matrixBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    void StaticGeometryDecorator::rasterizeShadowGeometry() const
    {
      m_matrixBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      m_renderNode->rasterizeShadowGeometry();

      m_matrixBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    void StaticGeometryDecorator::rasterizeReflectiveShadowGeometry() const
    {
      m_matrixBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      m_renderNode->rasterizeReflectiveShadowGeometry();

      m_matrixBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    void StaticGeometryDecorator::rasterizeIndexGeometry() const
    {
      m_matrixBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      m_renderNode->rasterizeIndexGeometry();

      m_matrixBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    void StaticGeometryDecorator::rasterizeIndirectLightingGeometry() const
    {
      m_matrixBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      m_renderNode->rasterizeIndirectLightingGeometry();

      m_matrixBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    void StaticGeometryDecorator::rasterizeGeometry() const
    {
      m_matrixBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      m_renderNode->rasterizeGeometry();

      m_matrixBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    void StaticGeometryDecorator::updateBuffer()
    {
      unsigned int instanceNumber = getInstanceNumber();

      m_matrixBuffer.setMemoryFence();

      unsigned int instanceIndex = 0;
      resetInstanceIterator();
      while (!isEndInstanceIterator())
      {
        const xBar::StaticGeometryContainer &staticGeometryContainer = dynamic_cast<const xBar::StaticGeometryContainer&>(incInstanceIterator());
        m_matrixBuffer.setData(sizeof(util::Matrix<float, 4>) * instanceIndex, sizeof(util::Matrix<float, 4>), &staticGeometryContainer.getTransformationMatrix()[0][0]);

        instanceIndex++;
      }

      m_renderNode->updateBuffer();

      m_matrixBuffer.syncWithFence();
    }
  }
}

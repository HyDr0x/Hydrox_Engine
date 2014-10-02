#include "Renderer/TreeNodes/RenderNodeDecorator/StaticGeometryDecorator.h"

#include "Renderer/Traverser/Traverser.h"

#include <XBar/StaticGeometryContainer.h>

namespace he
{
  namespace renderer
  {
    StaticGeometryDecorator::StaticGeometryDecorator(IRenderNode *renderNode) : ARenderNodeDecorator(renderNode)
    {
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

    void StaticGeometryDecorator::rasterizeGeometry() const
    {
      m_matrixBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      m_renderNode->rasterizeGeometry();

      m_matrixBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    void StaticGeometryDecorator::updateBuffer()
    {
      unsigned int instanceNumber = getInstanceNumber();

      if(hasInstanceNumberChanged())
      {
        m_matrixBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(util::Matrix<float, 4>) * instanceNumber, 0, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT, nullptr);
      }

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

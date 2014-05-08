#include "Renderer/TreeNodes/RenderNodeDecorator/StaticGeometryDecorator.h"

#include "Renderer/Resources/Material.h"
#include "Renderer/Resources/Mesh.h"
#include "Renderer/Resources/RenderShader.h"

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

    void StaticGeometryDecorator::frustumCulling()
    {
      m_matrixBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      m_renderNode->frustumCulling();

      m_matrixBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    void StaticGeometryDecorator::rasterizeGeometry()
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
      for(std::list<xBar::StaticGeometryContainer*>::iterator geometryIterator = getInstances().begin(); geometryIterator != getInstances().end(); geometryIterator++, instanceIndex++)
      {
        m_matrixBuffer.setData(sizeof(util::Matrix<float, 4>) * instanceIndex, sizeof(util::Matrix<float, 4>), &(*geometryIterator)->getTransformationMatrix()[0][0]);
      }

      m_renderNode->updateBuffer();

      m_matrixBuffer.syncWithFence();
    }
	}
}

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

    bool StaticGeometryDecorator::insertGeometry(xBar::StaticGeometryContainer& geometryContainer)
    {
      if(m_renderNode->insertGeometry(geometryContainer))
      {
        m_meshHandles[geometryContainer.getMeshHandle()].push_back(dynamic_cast<xBar::StaticGeometryContainer&>(geometryContainer));

        return true;
      }    

      return false;
    }

    bool StaticGeometryDecorator::removeGeometry(xBar::StaticGeometryContainer& geometryContainer)
    {
      if(m_renderNode->removeGeometry(geometryContainer))
      {
        std::list<xBar::StaticGeometryContainer>& geometryList = m_meshHandles[geometryContainer.getMeshHandle()];
        for(std::list<xBar::StaticGeometryContainer>::iterator geometryIterator = geometryList.begin(); geometryIterator != geometryList.end(); geometryIterator++)
        {
          if(geometryIterator->getHash() == geometryContainer.getHash())
          {
            geometryList.erase(geometryIterator);
            break;
          }
        }

        if(m_meshHandles[geometryContainer.getMeshHandle()].size() == 0)
        {
          m_meshHandles.erase(geometryContainer.getMeshHandle());
        }

        return true;
      }

      return false;
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

    bool StaticGeometryDecorator::isEmpty()
    {
      return m_meshHandles.size() == 0;
    }

    void StaticGeometryDecorator::updateBuffer()
    {
      unsigned int instanceNumber = getInstanceCount();

      if(hasGeometryChanged() || hasInstanceNumberChanged())
      {
        resizeBuffer(instanceNumber);
      }

      unsigned int instanceIndex = 0;
      for(std::map<util::ResourceHandle, std::list<xBar::StaticGeometryContainer>, Less>::iterator meshIterator = m_meshHandles.begin(); meshIterator != m_meshHandles.end(); meshIterator++)
      {
        for(std::list<xBar::StaticGeometryContainer>::iterator geometryIterator = meshIterator->second.begin(); geometryIterator != meshIterator->second.end(); geometryIterator++, instanceIndex++)
        {
          fillCaches(*geometryIterator, instanceIndex);
        }
      }

      fillBuffer(instanceNumber);

      m_renderNode->updateBuffer();
    }

    void StaticGeometryDecorator::resizeBuffer(unsigned int instanceNumber)
    {
      m_matrixCache.resize(sizeof(util::Matrix<float, 4>) * instanceNumber);
      m_matrixBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(util::Matrix<float, 4>) * instanceNumber, 0, GL_DYNAMIC_DRAW, nullptr);
    }

    void StaticGeometryDecorator::fillCaches(xBar::StaticGeometryContainer& geometryContainer, unsigned int instanceCounter)
    {
      memcpy(&m_matrixCache[instanceCounter * sizeof(util::Matrix<float, 4>)], &geometryContainer.getTransformationMatrix()[0], sizeof(util::Matrix<float, 4>));
    }

    void StaticGeometryDecorator::fillBuffer(unsigned int instanceNumber)
    {
      m_matrixBuffer.setData(0, sizeof(util::Matrix<float, 4>) * instanceNumber, &m_matrixCache[0]);
    }
	}
}

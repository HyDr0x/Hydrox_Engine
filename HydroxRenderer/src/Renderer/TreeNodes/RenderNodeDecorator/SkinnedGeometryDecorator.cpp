#include "Renderer/TreeNodes/RenderNodeDecorator/SkinnedGeometryDecorator.h"

#include "Renderer/Resources/Material.h"
#include "Renderer/Resources/Mesh.h"
#include "Renderer/Resources/RenderShader.h"

#include "Renderer/Traverser/Traverser.h"

#include <XBar/SkinnedGeometryContainer.h>

namespace he
{
	namespace renderer
	{
    SkinnedGeometryDecorator::SkinnedGeometryDecorator(IRenderNode *renderNode, unsigned int maxBones) : ARenderNodeDecorator(renderNode), m_maxBones(maxBones)
    {
    }

    SkinnedGeometryDecorator::~SkinnedGeometryDecorator()
    {
    }

    bool SkinnedGeometryDecorator::insertGeometry(xBar::StaticGeometryContainer& geometryContainer)
    {
      if(m_renderNode->insertGeometry(geometryContainer))
      {
        m_geometryContainer.push_back(dynamic_cast<xBar::SkinnedGeometryContainer&>(geometryContainer));

        return true;
      }    

      return false;
    }

    unsigned int SkinnedGeometryDecorator::removeGeometry(xBar::StaticGeometryContainer& geometryContainer)
    {
      unsigned int instanceIndex = m_renderNode->removeGeometry(geometryContainer);
      if(instanceIndex != ~0)
      {
        unsigned int index = 0;
        for(std::list<xBar::SkinnedGeometryContainer>::iterator geometryIterator = m_geometryContainer.begin(); geometryIterator != m_geometryContainer.end(); geometryIterator++, index++)
        {
          if(index == instanceIndex)
          {
            m_geometryContainer.erase(geometryIterator);
            break;
          }
        }
      }

      return instanceIndex;
    }

    void SkinnedGeometryDecorator::frustumCulling()
    {
      m_bboxMatrixBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      m_renderNode->frustumCulling();

      m_bboxMatrixBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    void SkinnedGeometryDecorator::rasterizeGeometry()
    {
      m_matrixBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      m_renderNode->rasterizeGeometry();

      m_matrixBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    bool SkinnedGeometryDecorator::isEmpty()
    {
      return m_geometryContainer.size() == 0;
    }

    void SkinnedGeometryDecorator::updateBuffer()
    {
      unsigned int instanceNumber = getInstanceNumber();

      if(hasInstanceNumberChanged())
      {
        resizeBuffer(instanceNumber);
      }

      m_matrixBuffer.setMemoryFence();
      m_bboxMatrixBuffer.setMemoryFence();

      fillBuffer();

      m_renderNode->updateBuffer();

      m_bboxMatrixBuffer.syncWithFence();
      m_matrixBuffer.syncWithFence();
    }

    void SkinnedGeometryDecorator::resizeBuffer(unsigned int instanceNumber)
    {
      m_matrixBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(util::Matrix<float, 4>) * m_maxBones * instanceNumber, 0, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT, nullptr);
      m_bboxMatrixBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(util::Matrix<float, 4>) * instanceNumber, 0, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT, nullptr);
    }

    void SkinnedGeometryDecorator::fillBuffer()
    {
      unsigned int instanceIndex = 0;
      for (std::list<xBar::SkinnedGeometryContainer>::iterator geometryIterator = m_geometryContainer.begin(); geometryIterator != m_geometryContainer.end(); geometryIterator++, instanceIndex++)
      {
        std::vector<util::Matrix<float, 4>>& skinningMatrices = geometryIterator->getSkinningMatrices();

        unsigned int size = sizeof(util::Matrix<float, 4>) * skinningMatrices.size();
        unsigned int offset = sizeof(util::Matrix<float, 4>) * m_maxBones * instanceIndex;

        m_matrixBuffer.setData(offset, size, &(skinningMatrices[0][0][0]));
        m_bboxMatrixBuffer.setData(instanceIndex * sizeof(util::Matrix<float, 4>), sizeof(util::Matrix<float, 4>), &geometryIterator->getTransformationMatrix()[0][0]);
      }
    }
	}
}

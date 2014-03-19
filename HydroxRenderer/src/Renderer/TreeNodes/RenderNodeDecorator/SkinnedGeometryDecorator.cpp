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
        m_meshHandles[geometryContainer.getMeshHandle()].push_back(dynamic_cast<xBar::SkinnedGeometryContainer&>(geometryContainer));

        return true;
      }    

      return false;
    }

    bool SkinnedGeometryDecorator::removeGeometry(xBar::StaticGeometryContainer& geometryContainer)
    {
      if(m_renderNode->removeGeometry(geometryContainer))
      {
        std::list<xBar::SkinnedGeometryContainer>& geometryList = m_meshHandles[geometryContainer.getMeshHandle()];
        for(std::list<xBar::SkinnedGeometryContainer>::iterator geometryIterator = geometryList.begin(); geometryIterator != geometryList.end(); geometryIterator++)
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
      return m_meshHandles.size() == 0;
    }

    void SkinnedGeometryDecorator::updateBuffer()
    {
      unsigned int instanceNumber = getInstanceCount();

      if(hasGeometryChanged() || hasInstanceNumberChanged())
      {
        resizeBuffer(instanceNumber);
      }

      unsigned int instanceIndex = 0;
      for(std::map<util::ResourceHandle, std::list<xBar::SkinnedGeometryContainer>, Less>::iterator meshIterator = m_meshHandles.begin(); meshIterator != m_meshHandles.end(); meshIterator++)
      {
        for(std::list<xBar::SkinnedGeometryContainer>::iterator geometryIterator = meshIterator->second.begin(); geometryIterator != meshIterator->second.end(); geometryIterator++, instanceIndex++)
        {
          fillCaches(*geometryIterator, instanceIndex);
        }
      }

      fillBuffer(instanceNumber);

      m_renderNode->updateBuffer();
    }

    void SkinnedGeometryDecorator::resizeBuffer(unsigned int instanceNumber)
    {
      m_matrixCache.resize(sizeof(util::Matrix<float, 4>) * instanceNumber * m_maxBones);
      m_matrixBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, m_matrixCache.size(), 0, GL_DYNAMIC_DRAW, nullptr);

      m_bboxMatrixCache.resize(sizeof(util::Matrix<float, 4>) * instanceNumber);
      m_bboxMatrixBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, m_bboxMatrixCache.size(), 0, GL_DYNAMIC_DRAW, nullptr);
    }

    void SkinnedGeometryDecorator::fillCaches(xBar::SkinnedGeometryContainer& geometryContainer, unsigned int instanceCounter)
    {
      std::vector<util::Matrix<float, 4>>& skinningMatrices = geometryContainer.getSkinningMatrices();

      unsigned int size = sizeof(util::Matrix<float, 4>) * skinningMatrices.size();
      unsigned int offset = sizeof(util::Matrix<float, 4>) * m_maxBones * instanceCounter;

      memcpy(&m_matrixCache[offset], &(skinningMatrices[0][0][0]), size);

      memcpy(&m_bboxMatrixCache[instanceCounter * sizeof(util::Matrix<float, 4>)], &geometryContainer.getTransformationMatrix()[0], sizeof(util::Matrix<float, 4>));
    }

    void SkinnedGeometryDecorator::fillBuffer(unsigned int instanceNumber)
    {
      m_matrixBuffer.setData(0, sizeof(util::Matrix<float, 4>) * m_maxBones * instanceNumber, &m_matrixCache[0]);
      m_bboxMatrixBuffer.setData(0, sizeof(util::Matrix<float, 4>) * instanceNumber, &m_bboxMatrixCache[0]);
    }
	}
}

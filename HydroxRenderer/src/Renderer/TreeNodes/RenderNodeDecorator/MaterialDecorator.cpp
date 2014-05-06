#include "Renderer/TreeNodes/RenderNodeDecorator/MaterialDecorator.h"

#include <XBar/StaticGeometryContainer.h>

#include "Renderer/Resources/Material.h"

#include "Renderer/Traverser/Traverser.h"

namespace he
{
	namespace renderer
	{
    MaterialDecorator::MaterialDecorator(IRenderNode *renderNode, unsigned int maxMaterials, util::SingletonManager *singletonManager) : 
      ARenderNodeDecorator(renderNode), 
      m_maxMaterials(maxMaterials),
      m_materialCount(0),
      m_materialNumberChanged(false)
    {
      m_materialManager = singletonManager->getService<MaterialManager>();

      m_materialBuffer.createBuffer(sizeof(Material::MaterialData) * maxMaterials, GL_STATIC_DRAW);
    }

    MaterialDecorator::~MaterialDecorator()
    {
    }

    bool MaterialDecorator::preTraverse(Traverser* traverser)
    {
      return traverser->preTraverse(this);
    }

    void MaterialDecorator::postTraverse(Traverser* traverser)
    {
      return traverser->postTraverse(this);
    }

    bool MaterialDecorator::insertGeometry(xBar::StaticGeometryContainer& geometryContainer)
    {
      if(!m_materialHandles.count(geometryContainer.getMaterialHandle()))
      {
        if(m_materialCount < m_maxMaterials && m_renderNode->insertGeometry(geometryContainer))
        {
          m_materialNumberChanged = true;

          m_materialHandles[geometryContainer.getMaterialHandle()].instanceNumber = 1;
          m_materialHandlesPerInstance.push_back(geometryContainer.getMaterialHandle());

          m_materialCount++;

          return true;
        }
      }
      else
      {
        if(m_renderNode->insertGeometry(geometryContainer))
        {
          m_materialHandles[geometryContainer.getMaterialHandle()].instanceNumber++;
          m_materialHandlesPerInstance.push_back(geometryContainer.getMaterialHandle());

          return true;
        }
      }

      return false;
    }

    unsigned int MaterialDecorator::removeGeometry(xBar::StaticGeometryContainer& geometryContainer)
    {
      unsigned int instanceIndex = m_renderNode->removeGeometry(geometryContainer);
      if(instanceIndex != ~0)
      {
        unsigned int index = 0;
        for(std::list<util::ResourceHandle>::const_iterator instanceIterator = m_materialHandlesPerInstance.begin(); instanceIterator != m_materialHandlesPerInstance.end(); instanceIterator++, index++)
        {
          if(index == instanceIndex)
          {
            m_materialHandles[geometryContainer.getMaterialHandle()].instanceNumber--;
            
            if(m_materialHandles[geometryContainer.getMaterialHandle()].instanceNumber == 0)
            {
              m_materialNumberChanged = true;
              m_materialHandles.erase(geometryContainer.getMaterialHandle());

              m_materialCount--;
            }

            m_materialHandlesPerInstance.erase(instanceIterator);
            break;
          }
        }
      }

      return instanceIndex;
    }

    void MaterialDecorator::rasterizeGeometry()
    {
      m_materialIndexBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      m_materialBuffer.bindBuffer(1);

      m_renderNode->rasterizeGeometry();

      m_materialBuffer.unBindBuffer();
      m_materialIndexBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
    }

    void MaterialDecorator::updateBuffer()
    {
      if(m_materialNumberChanged)
      {
        resizeMaterialBuffer();
        resizeMaterialIndexBuffer();

        m_materialNumberChanged = false;
      }
      else if(hasInstanceNumberChanged())
      {
        resizeMaterialIndexBuffer();
      }

      m_renderNode->updateBuffer();
    }

    void MaterialDecorator::resizeMaterialBuffer()
    {
      unsigned int instanceNumber = getInstanceNumber();

      unsigned int index = 0;
      for(std::map<util::ResourceHandle, MaterialMemoryManager, Less>::iterator instanceIterator = m_materialHandles.begin(); instanceIterator != m_materialHandles.end(); instanceIterator++, index++)
      {
        Material *material = m_materialManager->getObject(instanceIterator->first);
        m_materialBuffer.setData(m_materialCount * sizeof(Material::MaterialData), sizeof(Material::MaterialData), &material->getMaterialData());

        instanceIterator->second.bufferIndex = index;
      }

      m_materialBuffer.uploadData();
    }

    void MaterialDecorator::resizeMaterialIndexBuffer()
    {
      unsigned int instanceNumber = getInstanceNumber();

      m_materialIndexBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint) * instanceNumber, 0, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT, nullptr);

      m_materialIndexBuffer.setMemoryFence();

      unsigned int index = 0;
      for(std::list<util::ResourceHandle>::iterator instanceIterator = m_materialHandlesPerInstance.begin(); instanceIterator != m_materialHandlesPerInstance.end(); instanceIterator++, index++)
      {
        m_materialIndexBuffer.setData(sizeof(GLuint) * index, sizeof(GLuint), &m_materialHandles[*instanceIterator].bufferIndex);
      }

      m_materialIndexBuffer.syncWithFence();
    }
  }
}
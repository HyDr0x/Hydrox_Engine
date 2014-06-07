#include "Renderer/TreeNodes/RenderNodeDecorator/MaterialDecorator.h"

#include <XBar/StaticGeometryContainer.h>
#include <XBar/SkinnedGeometryContainer.h>

#include "Renderer/Resources/Material.h"

#include "Renderer/Traverser/Traverser.h"

namespace he
{
  namespace renderer
  {
    MaterialDecorator::MaterialDecorator(IRenderNode *renderNode, util::SingletonManager *singletonManager) : 
      ARenderNodeDecorator(renderNode), 
      m_materialCount(0),
      m_materialNumberChanged(false)
    {
      m_materialManager = singletonManager->getService<MaterialManager>();

      m_materialBuffer.createBuffer(sizeof(Material::MaterialData) * getMaxMaterials(), GL_STATIC_DRAW);
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

    bool MaterialDecorator::insertGeometry(const xBar::SkinnedGeometryContainer& geometryContainer)
    {
      if(!m_materialHandles.count(geometryContainer.getMaterialHandle()))
      {
        if(m_materialCount < getMaxMaterials() && m_renderNode->insertGeometry(geometryContainer))
        {
          m_materialNumberChanged = true;

          m_materialHandles[geometryContainer.getMaterialHandle()].instanceNumber = 1;

          m_materialCount++;

          return true;
        }
      }
      else
      {
        if(m_renderNode->insertGeometry(geometryContainer))
        {
          m_materialHandles[geometryContainer.getMaterialHandle()].instanceNumber++;

          return true;
        }
      }

      return false;
    }

    bool MaterialDecorator::insertGeometry(const xBar::StaticGeometryContainer& geometryContainer)
    {
      if(!m_materialHandles.count(geometryContainer.getMaterialHandle()))
      {
        if(m_materialCount < getMaxMaterials() && m_renderNode->insertGeometry(geometryContainer))
        {
          m_materialNumberChanged = true;

          m_materialHandles[geometryContainer.getMaterialHandle()].instanceNumber = 1;

          m_materialCount++;

          return true;
        }
      }
      else
      {
        if(m_renderNode->insertGeometry(geometryContainer))
        {
          m_materialHandles[geometryContainer.getMaterialHandle()].instanceNumber++;

          return true;
        }
      }

      return false;
    }

    bool MaterialDecorator::removeGeometry(const xBar::StaticGeometryContainer& geometryContainer)
    {
      bool deleted = m_renderNode->removeGeometry(geometryContainer);
      if(deleted)
      {
        for(std::list<const xBar::StaticGeometryContainer*>::const_iterator instanceIterator = getInstances().begin(); instanceIterator != getInstances().end(); instanceIterator++)
        {
          if(geometryContainer == (**instanceIterator))
          {
            m_materialHandles[geometryContainer.getMaterialHandle()].instanceNumber--;
            
            if(m_materialHandles[geometryContainer.getMaterialHandle()].instanceNumber == 0)
            {
              m_materialNumberChanged = true;
              m_materialHandles.erase(geometryContainer.getMaterialHandle());

              m_materialCount--;
            }
            break;
          }
        }
      }

      return deleted;
    }

    void MaterialDecorator::rasterizeGeometry() const
    {
      m_materialIndexBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      m_materialBuffer.bindBuffer(1);

      m_renderNode->rasterizeGeometry();

      m_materialBuffer.unBindBuffer(1);
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
      for(std::list<const xBar::StaticGeometryContainer*>::const_iterator instanceIterator = getInstances().begin(); instanceIterator != getInstances().end(); instanceIterator++, index++)
      {
        m_materialIndexBuffer.setData(sizeof(GLuint) * index, sizeof(GLuint), &m_materialHandles[(*instanceIterator)->getMeshHandle()].bufferIndex);
      }

      m_materialIndexBuffer.syncWithFence();
    }
  }
}
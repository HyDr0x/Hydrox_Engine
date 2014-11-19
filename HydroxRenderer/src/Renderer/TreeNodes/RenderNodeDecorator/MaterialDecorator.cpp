#include "Renderer/TreeNodes/RenderNodeDecorator/MaterialDecorator.h"

#include <XBar/StaticGeometryContainer.h>
#include <XBar/SkinnedGeometryContainer.h>

#include <DataBase/Material.h>

#include "Renderer/Traverser/Traverser.h"

namespace he
{
  namespace renderer
  {
    MaterialDecorator::MaterialDecorator(IRenderGroup *renderNode, util::SingletonManager *singletonManager) :
      ARenderNodeDecorator(renderNode), 
      m_materialCount(0),
      m_materialNumberChanged(false)
    {
      m_materialManager = singletonManager->getService<db::MaterialManager>();

      m_materialBuffer.createBuffer(sizeof(db::Material::MaterialData) * getMaxMaterials(), GL_STATIC_DRAW);
    }

    MaterialDecorator::~MaterialDecorator()
    {
    }

    bool MaterialDecorator::insertGeometry(const xBar::IGeometryContainer& geometryContainer)
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

    bool MaterialDecorator::removeGeometry(const xBar::IGeometryContainer& geometryContainer)
    {
      bool deleted = m_renderNode->removeGeometry(geometryContainer);
      if(deleted)
      {
        resetInstanceIterator();
        while (!isEndInstanceIterator())
        {
          const xBar::IGeometryContainer& instance = incInstanceIterator();
          if (geometryContainer == instance)
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

    void MaterialDecorator::rasterizeShadowGeometry() const
    {
      m_materialIndexBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      m_materialBuffer.bindBuffer(1);

      m_renderNode->rasterizeShadowGeometry();

      m_materialBuffer.unBindBuffer(1);
      m_materialIndexBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
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
      for (std::map<util::ResourceHandle, MaterialIndexData, Less>::iterator instanceIterator = m_materialHandles.begin(); instanceIterator != m_materialHandles.end(); instanceIterator++, index++)
      {
        db::Material *material = m_materialManager->getObject(instanceIterator->first);
        m_materialBuffer.setData(index * sizeof(db::Material::MaterialData), sizeof(db::Material::MaterialData), &material->getMaterialData());

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
      resetInstanceIterator();
      while (!isEndInstanceIterator())
      {
        const xBar::IGeometryContainer& instance = incInstanceIterator();
        
        m_materialIndexBuffer.setData(sizeof(GLuint)* index, sizeof(GLuint), &m_materialHandles[instance.getMeshHandle()].bufferIndex);
        index++;
      }

      m_materialIndexBuffer.syncWithFence();
    }
  }
}
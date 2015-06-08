#include "Renderer/Renderdata/MaterialRenderData.h"

#include <XBar/StaticGeometryContainer.h>
#include <XBar/SkinnedGeometryContainer.h>

#include <DataBase/Material.h>

#include "Renderer/Traverser/Traverser.h"

namespace he
{
  namespace renderer
  {
    MaterialRenderData::MaterialRenderData(util::SingletonManager *singletonManager, std::vector<std::vector<util::ResourceHandle>> textureHandles) :
      IMaterialRenderData(singletonManager, textureHandles)
    {}

    MaterialRenderData::~MaterialRenderData()
    {
    }

    bool MaterialRenderData::canBeInserted(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer)
    {
      if(!m_materialHandles.count(geometryContainer->getMaterialHandle()))
      {
        db::Material *material = m_materialManager->getObject(geometryContainer->getMaterialHandle());
        return m_materialCount < m_options->maxMaterialNumber && material->getTextureHandles() == m_textureHandles;
      }
      
      return true;
    }

    void MaterialRenderData::insertGeometry(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer)
    {
      if(!m_materialHandles.count(geometryContainer->getMaterialHandle()))
      {
        m_updateMaterialData = true;

        m_materialHandles[geometryContainer->getMaterialHandle()].instanceNumber = 1;

        m_materialCount++;
      }
      else
      {
        m_materialHandles[geometryContainer->getMaterialHandle()].instanceNumber++;
      }
    }

    void MaterialRenderData::removeGeometry(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer)
    {
      for(std::map<util::ResourceHandle, MaterialIndexData, util::ResourceHandle::Less>::iterator it = m_materialHandles.begin(); it != m_materialHandles.end(); it++)
      {
        if(geometryContainer->getMaterialHandle() == it->first)
        {
          m_materialHandles[geometryContainer->getMaterialHandle()].instanceNumber--;
            
          if(m_materialHandles[geometryContainer->getMaterialHandle()].instanceNumber == 0)
          {
            m_updateMaterialData = true;
            m_materialHandles.erase(geometryContainer->getMaterialHandle());

            m_materialCount--;
          }
          break;
        }
      }
    }

    void MaterialRenderData::bindMaterialBuffer() const
    {
      m_materialIndexBuffer.bindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
      m_materialBuffer.bindBuffer(1);
    }

    void MaterialRenderData::unbindMaterialBuffer() const
    {
      m_materialBuffer.unBindBuffer(1);
      m_materialIndexBuffer.unbindBuffer(GL_SHADER_STORAGE_BUFFER, 1);
    }

    const std::vector<std::vector<util::ResourceHandle>>& MaterialRenderData::getTextureHandles() const
    {
      return m_textureHandles;
    }

    void MaterialRenderData::updateBuffer(IInstanceRenderData& renderData)
    {
      for(std::map<util::ResourceHandle, uint64_t, util::ResourceHandle::Less>::iterator materialIterator = m_materialHashes.begin(); materialIterator != m_materialHashes.end(); materialIterator++)
      {
        db::Material *material = m_materialManager->getObject(materialIterator->first);
        if(material->getHash() != materialIterator->second)
        {
          m_updateMaterialData = true;//we need to update the material data
          break;
        }
      }

      if(m_updateMaterialData)
      {
        resizeMaterialBuffer(renderData);
        resizeMaterialIndexBuffer(renderData);

        m_updateMaterialData = false;
      }
      else if(renderData.hasInstanceNumberChanged())
      {
        resizeMaterialIndexBuffer(renderData);

        GPUImmutableBuffer::syncWithWrittenData();
      }
    }

    void MaterialRenderData::resizeMaterialBuffer(IInstanceRenderData& renderData)
    {
      unsigned int instanceNumber = renderData.getInstanceNumber();

      unsigned int index = 0;
      for(std::map<util::ResourceHandle, MaterialIndexData, util::ResourceHandle::Less>::iterator materialIterator = m_materialHandles.begin(); materialIterator != m_materialHandles.end(); materialIterator++, index++)
      {
        db::Material *material = m_materialManager->getObject(materialIterator->first);
        m_materialBuffer.setData(index * sizeof(db::Material::MaterialData), sizeof(db::Material::MaterialData), &material->getMaterialData());

        m_materialHashes[materialIterator->first] = material->getHash();//update the hash value

        materialIterator->second.bufferIndex = index;
      }

      m_materialBuffer.uploadData();
    }

    void MaterialRenderData::resizeMaterialIndexBuffer(IInstanceRenderData& renderData)
    {
      unsigned int instanceNumber = renderData.getInstanceNumber();

      unsigned int index = 0;
      renderData.resetInstanceIterator();
      while(!renderData.isEndInstanceIterator())
      {
        const xBar::IGeometryContainer& instance = renderData.incInstanceIterator();
        
        m_materialIndexBuffer.setData(sizeof(GLuint)* index, sizeof(GLuint), &m_materialHandles[instance.getMaterialHandle()].bufferIndex);
        index++;
      }
    }
  }
}
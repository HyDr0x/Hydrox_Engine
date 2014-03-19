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
      m_materialCount(0)
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
        bool inserted = m_renderNode->insertGeometry(geometryContainer);
        if(m_materialCount >= m_maxMaterials || !inserted)
        {
          return false;
        }

        m_materialHandles[geometryContainer.getMaterialHandle()] = m_materialCount;

        m_meshHandles[geometryContainer.getMeshHandle()].push_back(std::pair<uint64_t, unsigned int>(geometryContainer.getHash(), m_materialCount));

        Material *material = m_materialManager->getObject(geometryContainer.getMaterialHandle());
        m_materialBuffer.setData(&material->getMaterialData(), m_materialCount * sizeof(Material::MaterialData), sizeof(Material::MaterialData));

        m_materialCount++;
      }
      else
      {
        if(m_renderNode->insertGeometry(geometryContainer))
        {
          m_meshHandles[geometryContainer.getMeshHandle()].push_back(std::pair<uint64_t, unsigned int>(geometryContainer.getHash(), m_materialHandles[geometryContainer.getMaterialHandle()]));

          return true;
        }

        return false;
      }
    }

    bool MaterialDecorator::removeGeometry(xBar::StaticGeometryContainer& geometryContainer)
    {
      if(m_renderNode->removeGeometry(geometryContainer))
      {
        m_meshHandles[geometryContainer.getMeshHandle()].remove(std::pair<uint64_t, unsigned int>(geometryContainer.getHash(), m_materialHandles[geometryContainer.getMaterialHandle()]));

        return true;
      }

      return false;
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
      if(hasGeometryChanged() || hasInstanceNumberChanged())
      {
        resizeBuffer();
      }

      m_renderNode->updateBuffer();
    }

    void MaterialDecorator::resizeBuffer()
    {
      unsigned int instanceNumber = getInstanceCount();

      std::vector<GLuint> materialIndexData(instanceNumber);

      unsigned int meshIndex = 0;
      unsigned int instanceIndex = 0;
      for(MeshMaterialHandles::const_iterator meshIterator = m_meshHandles.begin(); meshIterator != m_meshHandles.end(); meshIterator++, meshIndex++)
      {
        for(std::list<std::pair<uint64_t, unsigned int>>::const_iterator geometryIterator = meshIterator->second.begin(); geometryIterator != meshIterator->second.end(); geometryIterator++, instanceIndex++)
        {
          materialIndexData[instanceIndex] = geometryIterator->second;
        }
      }

      m_materialIndexBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint) * instanceNumber, 0, GL_STATIC_DRAW, &materialIndexData[0]);

      m_materialBuffer.uploadData();
    }
  }
}
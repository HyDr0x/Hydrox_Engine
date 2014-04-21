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

        m_meshHandles.push_back(m_materialCount);

        Material *material = m_materialManager->getObject(geometryContainer.getMaterialHandle());
        m_materialBuffer.setData(&material->getMaterialData(), m_materialCount * sizeof(Material::MaterialData), sizeof(Material::MaterialData));

        m_materialCount++;
      }
      else
      {
        if(m_renderNode->insertGeometry(geometryContainer))
        {
          m_meshHandles.push_back(m_materialHandles[geometryContainer.getMaterialHandle()]);

          return true;
        }

        return false;
      }
    }

    unsigned int MaterialDecorator::removeGeometry(xBar::StaticGeometryContainer& geometryContainer)
    {
      unsigned int instanceIndex = m_renderNode->removeGeometry(geometryContainer);
      if(instanceIndex != ~0)
      {
        unsigned int index = 0;
        for(std::list<unsigned int>::const_iterator instanceIterator = m_meshHandles.begin(); instanceIterator != m_meshHandles.end(); instanceIterator++, index++)
        {
          if(index == instanceIndex)
          {
            m_meshHandles.erase(instanceIterator);
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
      if(hasInstanceNumberChanged())
      {
        resizeBuffer();
      }

      m_renderNode->updateBuffer();
    }

    void MaterialDecorator::resizeBuffer()
    {
      unsigned int instanceNumber = getInstanceNumber();

      std::vector<GLuint> materialIndexData(instanceNumber);

      unsigned int index = 0;
      for(std::list<unsigned int>::const_iterator instanceIterator = m_meshHandles.begin(); instanceIterator != m_meshHandles.end(); instanceIterator++, index++)
      {
        materialIndexData[index] = *instanceIterator;
      }

      m_materialIndexBuffer.createBuffer(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint) * instanceNumber, 0, GL_STATIC_DRAW, &materialIndexData[0]);

      m_materialBuffer.uploadData();
    }
  }
}
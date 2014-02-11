#include "Renderer/RenderTree/RenderNode.h"

#include "Renderer/Resources/Material.h"
#include "Renderer/Resources/Mesh.h"
#include "Renderer/Resources/RenderShader.h"

#include "Renderer/RenderTree/InsertGeometryTraverser.h"

namespace he
{
	namespace renderer
	{
    RenderNode::RenderNode() : m_maxMaterials(512)
    {
      GLuint size = sizeof(Material::MaterialData) * m_maxMaterials;
      m_materialBuffer.createBuffer(size, GL_STATIC_DRAW);
    }

    RenderNode::~RenderNode()
    {
    }

    bool RenderNode::preTraverse(Traverser* traverser)
    {
      return traverser->preTraverse(this);
    }

    void RenderNode::postTraverse(Traverser* traverser)
    {
      traverser->postTraverse(this);
    }

    TreeNode* RenderNode::createNewNode(InsertGeometryTraverser* traverser)
    {
      return traverser->createNewNode(this);
    }

    void RenderNode::initialize(RenderShaderManager *renderShaderManager, MaterialManager *materialManager, ModelManager *modelManager)
    {
      m_renderShaderManager = renderShaderManager;
      m_materialManager = materialManager;
      m_modelManager = modelManager;
    }

    void RenderNode::insertGeometry(sg::GeoNode *node)
    {
      m_geoNodes.push_back(node);
      m_geometryChanged = true;
    }

    bool RenderNode::removeGeometry(sg::GeoNode *node)
    {
      m_geoNodes.remove(node);
      m_geometryChanged = true;

      return m_geoNodes.size() == 0;
    }

    void RenderNode::rasterizeGeometry()
    {
      if(m_geometryChanged)
      {
        m_geometryChanged = false;
        resizeBuffer();
      }

      Mesh *renderMesh;
      Material *renderMaterial;
      RenderShader *renderShader;

      for(std::list<sg::GeoNode*>::const_iterator geometryIterator = m_geoNodes.begin(); geometryIterator != m_geoNodes.end(); geometryIterator++)//Render 3D Objects
      {
        if((*geometryIterator)->getRenderable())
        {
          renderMesh = m_modelManager->getObject((*geometryIterator)->getMeshHandle());
          renderMaterial = m_materialManager->getObject((*geometryIterator)->getMaterialHandle());
          renderShader = m_renderShaderManager->getObject(renderMaterial->getShaderHandle());

          if((*geometryIterator)->isAnimatedGeoNode())
          {
            uploadMatrixBuffer(static_cast<sg::AnimatedGeoNode*>(*geometryIterator), renderShader);
          }
          else
          {
            uploadMatrixBuffer(static_cast<sg::GeoNode*>(*geometryIterator), renderShader);
          }

          renderMesh->render(0);
        }
      }
    }

    void RenderNode::resizeBuffer()
    {
      resizeMatrixBuffer();
      m_materialIndexBuffer.createBuffer(sizeof(Material::MaterialData) * m_geoNodes.size(), GL_STATIC_DRAW);

      Material *material;

      unsigned int geometryCounter = 0;
      unsigned int materialCounter = 0;
      for(std::list<sg::GeoNode*>::const_iterator geometryIterator = m_geoNodes.begin(); geometryIterator != m_geoNodes.end(); geometryIterator++, geometryCounter++)//Render 3D Objects
      {
        if(m_materialHandles.count((*geometryIterator)->getMaterialHandle().getID()) == 0)
        {
          m_materialHandles[(*geometryIterator)->getMaterialHandle().getID()] = materialCounter;

          material = m_materialManager->getObject((*geometryIterator)->getMaterialHandle());
          m_materialBuffer.setData(&material->getMaterialData(), materialCounter * sizeof(Material::MaterialData), sizeof(Material::MaterialData));

          materialCounter++;
        }

        m_materialIndexBuffer.setData(&m_materialHandles[(*geometryIterator)->getMaterialHandle().getID()], geometryCounter * sizeof(unsigned int), sizeof(unsigned int));

        if((*geometryIterator)->isAnimatedGeoNode())
        {
          fillMatrixBuffer(static_cast<sg::AnimatedGeoNode*>(*geometryIterator), geometryCounter);
        }
        else
        {
          fillMatrixBuffer(static_cast<sg::GeoNode*>(*geometryIterator), geometryCounter);
        }
      }

      m_materialBuffer.uploadData();
      m_materialIndexBuffer.uploadData();
    }
	}
}

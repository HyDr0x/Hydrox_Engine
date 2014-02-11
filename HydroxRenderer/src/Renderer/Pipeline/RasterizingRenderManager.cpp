#include "Renderer/Pipeline/RasterizerRenderManager.h"

namespace he
{
	namespace renderer
	{
    RasterizerRenderManager::RasterizerRenderManager()
    {
    }

    RasterizerRenderManager::~RasterizerRenderManager()
    {
      glDeleteBuffers(1, &m_boneMatricesBuffer);
      glDeleteBuffers(1, &m_dummyVBO);
      glDeleteVertexArrays(1, &m_simpleMeshVAO);

      delete m_geometryRasterizer;

      m_renderGeometry.clear();
      m_renderAnimatedGeometry.clear();
      m_renderBillboards.clear();
    }

    void RasterizerRenderManager::setClearColor(he::util::Vector<float, 4> color)
    {
	    glClearColor(color[0], color[1], color[2], color[3]);
    }

    void RasterizerRenderManager::resizeRenderWindow(unsigned int width, unsigned int height)
    {
      m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);

      glViewport(0, 0, width, height);
    }

    const size_t RasterizerRenderManager::getMaxSpriteLayer() const
    {
      return m_maxLayer;
    }

    void RasterizerRenderManager::addSprite(util::ResourceHandle spriteID, bool transparent)
    {
      if(transparent)
      {
        Sprite *renderSprite = m_spriteManager->getObject(spriteID);
        m_transparentSpriteIDs[renderSprite->getLayer()].push_back(spriteID);
      }
      else
      {
        m_opaqueSpriteIDs.push_back(spriteID);
      }
    }

    void RasterizerRenderManager::initialize(util::SingletonManager *singletonManager, GLfloat aspectRatio, size_t maxSpriteLayer)
    {
	    glEnable(GL_DEPTH_TEST);
	    glDepthMask(GL_TRUE);
	    glDepthFunc(GL_GREATER);

	    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	    glClearDepth(0.0f);

      m_computeShaderManager = singletonManager->getService<ComputeShaderManager>();
      m_renderShaderManager = singletonManager->getService<RenderShaderManager>();
      m_textureManager = singletonManager->getService<TextureManager>();
      m_billboardManager = singletonManager->getService<BillboardManager>();
      m_spriteManager = singletonManager->getService<SpriteManager>();
      m_modelManager = singletonManager->getService<ModelManager>();
      m_materialManager = singletonManager->getService<MaterialManager>();

      registerRenderComponentSlots(singletonManager->getService<util::EventManager>());

      m_geometryRasterizer = new GeometryRasterizer(singletonManager);

      m_aspectRatio = aspectRatio;
      m_maxLayer = maxSpriteLayer;

      m_transparentSpriteIDs.resize(m_maxLayer);

      glGenBuffers(1, &m_dummyVBO);
      glBindBuffer(GL_ARRAY_BUFFER, m_dummyVBO);
      glBufferData(GL_ARRAY_BUFFER, sizeof(float), nullptr, GL_STATIC_DRAW);
      glVertexAttribPointer(RenderShader::SPECIAL0, 1, GL_FLOAT, GL_FALSE, sizeof(float), NULL);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
    
      glGenBuffers(1, &m_boneMatricesBuffer);
      glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_boneMatricesBuffer);
      glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(util::Matrix<float, 4>) * 64, nullptr, GL_STATIC_DRAW);
      glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

      glGenVertexArrays(1, &m_simpleMeshVAO);
      glBindVertexArray(m_simpleMeshVAO);
      glVertexAttribFormat(RenderShader::POSITION, 3, GL_FLOAT, GL_FALSE, 0);
      glVertexAttribFormat(RenderShader::TEXTURE0, 2, GL_FLOAT, GL_FALSE, sizeof(util::Vector<float, 3>));
      glVertexAttribFormat(RenderShader::NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(util::Vector<float, 3>) + sizeof(util::Vector<float, 2>));
      glVertexAttribFormat(RenderShader::BINORMAL, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(util::Vector<float, 3>) + sizeof(util::Vector<float, 2>));
      glVertexAttribFormat(RenderShader::BONEWEIGHTS, 4, GL_FLOAT, GL_FALSE, 3 * sizeof(util::Vector<float, 3>) + sizeof(util::Vector<float, 2>));
      glVertexAttribFormat(RenderShader::BONEINDICES, 4, GL_FLOAT, GL_FALSE, sizeof(util::Vector<float, 4>) + 3 * sizeof(util::Vector<float, 3>) + sizeof(util::Vector<float, 2>));
      glVertexAttribFormat(RenderShader::COLOR, 4, GL_FLOAT, GL_FALSE, 2 * sizeof(util::Vector<float, 4>) + 3 * sizeof(util::Vector<float, 3>) + sizeof(util::Vector<float, 2>));

      glVertexAttribBinding(RenderShader::POSITION, 0);
      glVertexAttribBinding(RenderShader::TEXTURE0, 0);
      glVertexAttribBinding(RenderShader::NORMAL, 0);
      glVertexAttribBinding(RenderShader::BINORMAL, 0);
      glVertexAttribBinding(RenderShader::BONEWEIGHTS, 0);
      glVertexAttribBinding(RenderShader::BONEINDICES, 0);
      glVertexAttribBinding(RenderShader::COLOR, 0);
      glBindVertexArray(0);

      m_cameraParameterUBO.createBuffer(sizeof(util::Matrix<float, 4>) * 3 + sizeof(util::Vector<float, 4>), GL_DYNAMIC_DRAW);
    }

    void RasterizerRenderManager::initializeShader(util::ResourceHandle billboardShaderHandle, util::ResourceHandle spriteShaderHandle, util::ResourceHandle frustumCullingShaderHandle)
    {
      m_billboardShaderHandle = billboardShaderHandle;
      m_spriteShaderHandle = spriteShaderHandle;

      m_frustumCullingGPU.initialize(m_computeShaderManager, frustumCullingShaderHandle);
    }

    void RasterizerRenderManager::render(util::Matrix<float, 4>& viewMatrix, util::Matrix<float, 4>& projectionMatrix, util::Vector<float, 3>& cameraPosition)
    {
	    //oldRenderPath(viewMatrix, projectionMatrix, cameraPosition);

      m_geometryRasterizer->rasterizeGeometry(viewMatrix, projectionMatrix, cameraPosition);
    }

    void RasterizerRenderManager::oldRenderPath(util::Matrix<float, 4>& viewMatrix, util::Matrix<float, 4>& projectionMatrix, util::Vector<float, 3>& cameraPosition)
    {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

      util::Matrix<float, 4> viewProjectionMatrix = projectionMatrix * viewMatrix;
      util::Vector<float, 4> eyeVec = util::Vector<float, 4>(viewMatrix[3][0], viewMatrix[3][1], viewMatrix[3][2], 1.0f);

      m_cameraParameterUBO.setData(&viewMatrix[0][0], 0, sizeof(util::Matrix<float, 4>));
      m_cameraParameterUBO.setData(&projectionMatrix[0][0], sizeof(util::Matrix<float, 4>), sizeof(util::Matrix<float, 4>));
      m_cameraParameterUBO.setData(&viewProjectionMatrix[0][0], 2 * sizeof(util::Matrix<float, 4>), sizeof(util::Matrix<float, 4>));
      m_cameraParameterUBO.setData(&eyeVec[0], 3 * sizeof(util::Matrix<float, 4>), sizeof(util::Vector<float, 4>));
      m_cameraParameterUBO.uploadData();
      m_cameraParameterUBO.bindBuffer(0);

      ////////////////////////////////////////////CULL 3D Objects////////////////////////////////////////////

      Mesh *renderMesh;
      Material *renderMaterial;
      RenderShader *renderShader;
      Texture *renderTexture;

      std::vector<util::Matrix<float, 4>> transformMatrices(m_renderGeometry.size() + m_renderAnimatedGeometry.size());
      std::vector<util::Vector<float, 4>> bbMin(m_renderGeometry.size() + m_renderAnimatedGeometry.size());
      std::vector<util::Vector<float, 4>> bbMax(m_renderGeometry.size() + m_renderAnimatedGeometry.size());
      std::vector<unsigned int> culledAABB;

      unsigned int k = 0;

      /*//collect culling data for static 3D Objects
      for(std::list<sg::GeoNode*>::const_iterator geometryIterator = m_renderGeometry.begin(); geometryIterator != m_renderGeometry.end(); geometryIterator++, k++)
      {
        renderMesh = m_modelManager->getObject((*geometryIterator)->getMeshHandle());//SAVE ALLE MESHES, NO NEED TO CALL IT THEN ANYMORE, REPLACE THE OBSERVER THROUGH A ONE ELEMENT 

        transformMatrices[k] = (*geometryIterator)->getTransformationMatrix();
        memcpy(&bbMin[k][0], &renderMesh->getBBMin()[0], sizeof(util::Vector<float, 3>));//USE INSTANCING HERE, NEED BBOXES ONLY ONCE PER MESH
        memcpy(&bbMax[k][0], &renderMesh->getBBMax()[0], sizeof(util::Vector<float, 3>));
      }
      //collect culling data for skinned 3D Objects
      for(std::list<sg::AnimatedGeoNode*>::const_iterator animatedGeometryIterator = m_renderAnimatedGeometry.begin(); animatedGeometryIterator != m_renderAnimatedGeometry.end(); animatedGeometryIterator++, k++)
      {
        renderMesh = m_modelManager->getObject((*animatedGeometryIterator)->getMeshHandle());

        transformMatrices[k] = (*animatedGeometryIterator)->getTransformationMatrix();
        memcpy(&bbMin[k][0], &renderMesh->getBBMin()[0], sizeof(util::Vector<float, 3>));//USE INSTANCING HERE, NEED BBOXES ONLY ONCE PER MESH
        memcpy(&bbMax[k][0], &renderMesh->getBBMax()[0], sizeof(util::Vector<float, 3>));
      }*/

      //culledAABB.resize(m_renderGeometry.size() + m_renderAnimatedGeometry.size());
      //m_frustumCullingGPU.cullAABB(transformMatrices, bbMin, bbMax, culledAABB);
      culledAABB.resize(m_renderGeometry.size() + m_renderAnimatedGeometry.size(), 1);

	    ////////////////////////////////////////////RENDER 3D Objects////////////////////////////////////////////
    
      util::Matrix<float, 4> worldMatrix;
    
      glBindVertexArray(m_simpleMeshVAO);

      glEnableVertexAttribArray(RenderShader::POSITION);
      glEnableVertexAttribArray(RenderShader::TEXTURE0);

      k = 0;
      for(std::list<sg::GeoNode*>::const_iterator geometryIterator = m_renderGeometry.begin(); geometryIterator != m_renderGeometry.end(); geometryIterator++, k++)//Render 3D Objects
      {
        if((*geometryIterator)->getRenderable() && culledAABB[k])
        {
          renderMesh = m_modelManager->getObject((*geometryIterator)->getMeshHandle());
          renderMaterial = m_materialManager->getObject((*geometryIterator)->getMaterialHandle());
          renderShader = m_renderShaderManager->getObject(renderMaterial->getShaderHandle());
          renderTexture = m_textureManager->getObject(renderMaterial->getTextureHandle(Material::DIFFUSETEX, 0));

          renderShader->useShader();

          worldMatrix = (*geometryIterator)->getTransformationMatrix();
          renderShader->setUniform(17, GL_FLOAT_MAT4, &(worldMatrix[0][0]));

          renderTexture->setTexture(0, 0);

          renderMesh->render(0);
        }
      }

      glEnableVertexAttribArray(RenderShader::NORMAL);
      glEnableVertexAttribArray(RenderShader::BINORMAL);
      glEnableVertexAttribArray(RenderShader::BONEWEIGHTS);
      glEnableVertexAttribArray(RenderShader::BONEINDICES);

      for(std::list<sg::AnimatedGeoNode*>::const_iterator animatedGeometryIterator = m_renderAnimatedGeometry.begin(); animatedGeometryIterator != m_renderAnimatedGeometry.end(); animatedGeometryIterator++, k++)//Render 3D Objects
      {
        if((*animatedGeometryIterator)->getRenderable() && culledAABB[k])
        {
          renderMesh = m_modelManager->getObject((*animatedGeometryIterator)->getMeshHandle());
          renderMaterial = m_materialManager->getObject((*animatedGeometryIterator)->getMaterialHandle());
          renderShader = m_renderShaderManager->getObject(renderMaterial->getShaderHandle());
          renderTexture = m_textureManager->getObject(renderMaterial->getTextureHandle(Material::DIFFUSETEX, 0));

          renderShader->useShader();

          glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_boneMatricesBuffer);
          std::vector<util::Matrix<float, 4>> skinningMatrices = (*animatedGeometryIterator)->getSkinningMatrices();

          glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(util::Matrix<float, 4>) * skinningMatrices.size(), &(skinningMatrices[0][0][0]));
          glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
 
          glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_boneMatricesBuffer); 
          renderTexture->setTexture(0, 0);

          renderMesh->render(0);
        }
      }
      glBindVertexArray(0);

	    ////////////////////////////////////////////RENDER BILLBOARDS////////////////////////////////////////////
      Billboard *renderBillboard;

	    glEnable(GL_BLEND);
	    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      glEnableVertexAttribArray(RenderShader::SPECIAL0);
 
      RenderShader *billboardShader = m_renderShaderManager->getObject(m_billboardShaderHandle);
	    billboardShader->useShader();

      billboardShader->setUniform(1, GL_FLOAT_MAT4, &viewMatrix[0][0]);
      billboardShader->setUniform(2, GL_FLOAT_MAT4, &projectionMatrix[0][0]);

	    for(std::list<sg::BillboardNode*>::const_iterator billboarditerator = m_renderBillboards.begin(); billboarditerator != m_renderBillboards.end(); billboarditerator++)
	    {
        renderBillboard = m_billboardManager->getObject((*billboarditerator)->getBillboardIndex());
        if(renderBillboard->getRenderable())
        {
          renderTexture = m_textureManager->getObject(renderBillboard->getTextureID());

          renderTexture->setTexture(6, 0);

          util::Matrix<float, 4> worldMatrix = (*billboarditerator)->getTransformationMatrix();
		      util::Matrix<float, 3> tmpTexTrfMatrix = renderBillboard->getTexTransformationMatrix();
          util::Vector<float, 2> scale = renderBillboard->getScale();
		      util::Vector<float, 3> translate = renderBillboard->getPosition();
          billboardShader->setUniform(0, GL_FLOAT_MAT4, &worldMatrix[0][0]);
		      billboardShader->setUniform(3, GL_FLOAT_MAT3, &tmpTexTrfMatrix[0][0]);
		      billboardShader->setUniform(4, GL_FLOAT_VEC2, &scale[0]);
		      billboardShader->setUniform(5, GL_FLOAT_VEC3, &translate[0]);

          glBindBuffer(GL_ARRAY_BUFFER, m_dummyVBO);
          glDrawArrays(GL_POINTS, 0, 1);
        }
	    }
    
      glDisable(GL_BLEND);

	    ////////////////////////////////////////////RENDER OPAQUE 2D Sprites////////////////////////////////////////////

      glClear(GL_DEPTH_BUFFER_BIT);

      Sprite *renderSprite;

      RenderShader *spriteShader = m_renderShaderManager->getObject(m_spriteShaderHandle);
	    spriteShader->useShader();

	    for(std::list<util::ResourceHandle>::iterator spriteIDIterator = m_opaqueSpriteIDs.begin(); spriteIDIterator != m_opaqueSpriteIDs.end(); spriteIDIterator++)
      {
        renderSprite = m_spriteManager->getObject(*spriteIDIterator);
        if(renderSprite->getRenderable())
        {
          renderTexture = m_textureManager->getObject(renderSprite->getTextureID());

          renderTexture->setTexture(3, 0);
		
		      util::Matrix<float, 3> worldMatrix = renderSprite->getTransformationMatrix();
		      util::Matrix<float, 3> textureWorldMatrix = renderSprite->getTexTransformationMatrix();
          float z = renderSprite->getLayer() / (const float)m_maxLayer;
		      spriteShader->setUniform(0, GL_FLOAT_MAT3, &worldMatrix[0][0]);
		      spriteShader->setUniform(1, GL_FLOAT_MAT3, &textureWorldMatrix[0][0]);
          spriteShader->setUniform(2, GL_FLOAT, &z);

          glBindBuffer(GL_ARRAY_BUFFER, m_dummyVBO);
		      glDrawArrays(GL_POINTS, 0, 1);
        }
	    }

      ////////////////////////////////////////////RENDER TRANSPARENT 2D Sprites////////////////////////////////////////////

      for(unsigned int i = 0; i < m_transparentSpriteIDs.size(); i++)//resort all sprites according to their layer if their layer has been changed
      {
        for(std::list<util::ResourceHandle>::iterator spriteIDIterator = m_transparentSpriteIDs[i].begin(); spriteIDIterator != m_transparentSpriteIDs[i].end(); spriteIDIterator++)
        {
          renderSprite = m_spriteManager->getObject(*spriteIDIterator);
          if(renderSprite->getLayerChanged())
          {
            m_transparentSpriteIDs[renderSprite->getLayer()].push_back(*spriteIDIterator);
            renderSprite->spriteSorted();
            spriteIDIterator = m_transparentSpriteIDs[i].erase(spriteIDIterator);

            if(spriteIDIterator == m_transparentSpriteIDs[i].end())
            {
              break;
            }
          }
        }
      }

      glDepthMask(GL_FALSE);

      glEnable(GL_BLEND);
	    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      for(unsigned int i = 0; i < m_transparentSpriteIDs.size(); i++)
      {
        for(std::list<util::ResourceHandle>::iterator spriteIDIterator = m_transparentSpriteIDs[i].begin(); spriteIDIterator != m_transparentSpriteIDs[i].end(); spriteIDIterator++)
        {
          renderSprite = m_spriteManager->getObject(*spriteIDIterator);
          if(renderSprite->getRenderable())
          {
            renderTexture = m_textureManager->getObject(renderSprite->getTextureID());

            renderTexture->setTexture(3, 0);
		
		        util::Matrix<float, 3> worldMatrix = renderSprite->getTransformationMatrix();
		        util::Matrix<float, 3> textureWorldMatrix = renderSprite->getTexTransformationMatrix();
            float z = renderSprite->getLayer() / (const float)m_maxLayer;
		        spriteShader->setUniform(0, GL_FLOAT_MAT3, &worldMatrix[0][0]);
		        spriteShader->setUniform(1, GL_FLOAT_MAT3, &textureWorldMatrix[0][0]);
            spriteShader->setUniform(2, GL_FLOAT, &z);

            glBindBuffer(GL_ARRAY_BUFFER, m_dummyVBO);
		        glDrawArrays(GL_POINTS, 0, 1);
          }
	      }
      }

      glDisableVertexAttribArray(RenderShader::SPECIAL0);

      glDisable(GL_BLEND);

      glDepthMask(GL_TRUE);

      m_cameraParameterUBO.unBindBuffer();
    }

    void RasterizerRenderManager::addSprite(const std::list<util::ResourceHandle>& spriteIDList, bool transparent)
    {
      Sprite *renderSprite;

      for(std::list<util::ResourceHandle>::const_iterator sit = spriteIDList.begin(); sit != spriteIDList.end(); sit++)
      {
        if(transparent)
        {
          renderSprite = m_spriteManager->getObject(*sit);
          m_transparentSpriteIDs[renderSprite->getLayer()].push_back(*sit);
        }
        else
        {
          m_opaqueSpriteIDs.push_back(*sit);
        }
      }
    }

    void RasterizerRenderManager::removeSprite(util::ResourceHandle spriteID, bool transparent)
    {
      if(transparent)
      {
        Sprite *renderSprite = m_spriteManager->getObject(spriteID);
        m_transparentSpriteIDs[renderSprite->getLayer()].remove(spriteID);
      }
      else
      {
        m_opaqueSpriteIDs.remove(spriteID);
      }
    }

    void RasterizerRenderManager::removeAllSprites()
    {
      m_opaqueSpriteIDs.clear();
      m_transparentSpriteIDs.clear();
    }

    void RasterizerRenderManager::addRenderComponent(sg::BillboardNode *billboardNode)
    {
      m_renderBillboards.push_back(billboardNode);
    }

    void RasterizerRenderManager::addRenderComponent(sg::GeoNode *geoNode)
    {
      m_renderGeometry.push_back(geoNode);

      m_geometryRasterizer->insertGeometry(geoNode);
    }

    void RasterizerRenderManager::addRenderComponent(sg::AnimatedGeoNode *animatedGeoNode)
    {
      m_renderAnimatedGeometry.push_back(animatedGeoNode);
      m_geometryRasterizer->insertGeometry(animatedGeoNode);
    }

    void RasterizerRenderManager::addRenderComponent(sg::LightNode *lightNode)
    {
      m_renderLight.push_back(lightNode);
    }

    void RasterizerRenderManager::addRenderComponent(sg::ParticleNode *particleNode)
    {
      m_renderParticle.push_back(particleNode);
    }

    void RasterizerRenderManager::removeRenderComponent(sg::BillboardNode *billboardNode)
    {
      m_renderBillboards.remove(billboardNode);
    }

    void RasterizerRenderManager::removeRenderComponent(sg::GeoNode *geoNode)
    {
      m_renderGeometry.remove(geoNode);
      m_geometryRasterizer->removeGeometry(geoNode);
    }

    void RasterizerRenderManager::removeRenderComponent(sg::AnimatedGeoNode *animatedGeoNode)
    {
      m_renderAnimatedGeometry.remove(animatedGeoNode);
      m_geometryRasterizer->removeGeometry(animatedGeoNode);
    }

    void RasterizerRenderManager::removeRenderComponent(sg::LightNode *lightNode)
    {
      m_renderLight.remove(lightNode);
    }

    void RasterizerRenderManager::removeRenderComponent(sg::ParticleNode *particleNode)
    {
      m_renderParticle.remove(particleNode);
    }

    void RasterizerRenderManager::registerRenderComponentSlots(util::EventManager *eventManager)
    {
      eventManager->addNewSignal<void (*)(sg::BillboardNode *node)>(util::EventManager::OnAddBillboardNode);
      eventManager->addSlotToSignal<RasterizerRenderManager, void (*)(sg::BillboardNode *node), void (RasterizerRenderManager::*)(sg::BillboardNode *node)>(this, &RasterizerRenderManager::addRenderComponent, util::EventManager::OnAddBillboardNode);

      eventManager->addNewSignal<void (*)(sg::AnimatedGeoNode *node)>(util::EventManager::OnAddAnimatedGeometryNode);
      eventManager->addSlotToSignal<RasterizerRenderManager, void (*)(sg::AnimatedGeoNode *node), void (RasterizerRenderManager::*)(sg::AnimatedGeoNode *node)>(this, &RasterizerRenderManager::addRenderComponent, util::EventManager::OnAddAnimatedGeometryNode);

      eventManager->addNewSignal<void (*)(sg::GeoNode *node)>(util::EventManager::OnAddGeometryNode);
      eventManager->addSlotToSignal<RasterizerRenderManager, void (*)(sg::GeoNode *node), void (RasterizerRenderManager::*)(sg::GeoNode *node)>(this, &RasterizerRenderManager::addRenderComponent, util::EventManager::OnAddGeometryNode);

      eventManager->addNewSignal<void (*)(sg::LightNode *node)>(util::EventManager::OnAddLightNode);
      eventManager->addSlotToSignal<RasterizerRenderManager, void (*)(sg::LightNode *node), void (RasterizerRenderManager::*)(sg::LightNode *node)>(this, &RasterizerRenderManager::addRenderComponent, util::EventManager::OnAddLightNode);

      eventManager->addNewSignal<void (*)(sg::ParticleNode *node)>(util::EventManager::OnAddParticleTransmitterNode);
      eventManager->addSlotToSignal<RasterizerRenderManager, void (*)(sg::ParticleNode *node), void (RasterizerRenderManager::*)(sg::ParticleNode *node)>(this, &RasterizerRenderManager::addRenderComponent, util::EventManager::OnAddParticleTransmitterNode);

      eventManager->addNewSignal<void (*)(sg::BillboardNode *node)>(util::EventManager::OnRemoveBillboardNode);
      eventManager->addSlotToSignal<RasterizerRenderManager, void (*)(sg::BillboardNode *node), void (RasterizerRenderManager::*)(sg::BillboardNode *node)>(this, &RasterizerRenderManager::removeRenderComponent, util::EventManager::OnRemoveBillboardNode);

      eventManager->addNewSignal<void (*)(sg::AnimatedGeoNode *node)>(util::EventManager::OnRemoveAnimatedGeometryNode);
      eventManager->addSlotToSignal<RasterizerRenderManager, void (*)(sg::AnimatedGeoNode *node), void (RasterizerRenderManager::*)(sg::AnimatedGeoNode *node)>(this, &RasterizerRenderManager::removeRenderComponent, util::EventManager::OnRemoveAnimatedGeometryNode);

      eventManager->addNewSignal<void (*)(sg::GeoNode *node)>(util::EventManager::OnRemoveGeometryNode);
      eventManager->addSlotToSignal<RasterizerRenderManager, void (*)(sg::GeoNode *node), void (RasterizerRenderManager::*)(sg::GeoNode *node)>(this, &RasterizerRenderManager::removeRenderComponent, util::EventManager::OnRemoveGeometryNode);

      eventManager->addNewSignal<void (*)(sg::LightNode *node)>(util::EventManager::OnRemoveLightNode);
      eventManager->addSlotToSignal<RasterizerRenderManager, void (*)(sg::LightNode *node), void (RasterizerRenderManager::*)(sg::LightNode *node)>(this, &RasterizerRenderManager::removeRenderComponent, util::EventManager::OnRemoveLightNode);

      eventManager->addNewSignal<void (*)(sg::ParticleNode *node)>(util::EventManager::OnRemoveParticleTransmitterNode);
      eventManager->addSlotToSignal<RasterizerRenderManager, void (*)(sg::ParticleNode *node), void (RasterizerRenderManager::*)(sg::ParticleNode *node)>(this, &RasterizerRenderManager::removeRenderComponent, util::EventManager::OnRemoveParticleTransmitterNode);
    }
	}
}

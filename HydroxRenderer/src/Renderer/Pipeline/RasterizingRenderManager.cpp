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

    void RasterizerRenderManager::setBackfaceCulling(GLenum cullingMode)
    {
      glFrontFace(cullingMode);
    }

    void RasterizerRenderManager::setWireframe(bool wireFrame)
    {
      if(wireFrame)
      {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      }
      else
      {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      }
    }

    void RasterizerRenderManager::initialize(unsigned int maxMaterials, unsigned int maxGeometry, unsigned int maxBones, util::SingletonManager *singletonManager, GLfloat aspectRatio, size_t maxSpriteLayer, 
      util::ResourceHandle billboardShaderHandle, 
      util::ResourceHandle spriteShaderHandle, 
      util::ResourceHandle frustumCullingShaderHandle)
    {
      registerRenderComponentSlots(singletonManager->getService<util::EventManager>());

      m_geometryRasterizer.initialize(maxMaterials, maxGeometry, maxBones, singletonManager, frustumCullingShaderHandle);
      m_billboardRenderer.initialize(singletonManager, billboardShaderHandle);
      m_spriteRenderer.initialize(singletonManager, maxSpriteLayer, spriteShaderHandle);

      m_aspectRatio = aspectRatio;

      m_cameraParameterUBO.createBuffer(sizeof(util::Matrix<float, 4>) * 3 + sizeof(util::Vector<float, 4>), GL_DYNAMIC_DRAW);
    }

    void RasterizerRenderManager::render(util::Matrix<float, 4>& viewMatrix, util::Matrix<float, 4>& projectionMatrix, util::Vector<float, 3>& cameraPosition)
    {
      util::Matrix<float, 4> viewProjectionMatrix = projectionMatrix * viewMatrix;
      util::Vector<float, 4> eyeVec = util::Vector<float, 4>(viewMatrix[3][0], viewMatrix[3][1], viewMatrix[3][2], 1.0f);

      m_cameraParameterUBO.setData(&viewMatrix[0][0], 0, sizeof(util::Matrix<float, 4>));
      m_cameraParameterUBO.setData(&projectionMatrix[0][0], sizeof(util::Matrix<float, 4>), sizeof(util::Matrix<float, 4>));
      m_cameraParameterUBO.setData(&viewProjectionMatrix[0][0], 2 * sizeof(util::Matrix<float, 4>), sizeof(util::Matrix<float, 4>));
      m_cameraParameterUBO.setData(&eyeVec[0], 3 * sizeof(util::Matrix<float, 4>), sizeof(util::Vector<float, 4>));
      m_cameraParameterUBO.uploadData();
      m_cameraParameterUBO.bindBuffer(0);

      m_geometryRasterizer.rasterizeGeometry();
      m_billboardRenderer.render();
      m_spriteRenderer.render();
    }

    void RasterizerRenderManager::oldRenderPath(util::Matrix<float, 4>& viewMatrix, util::Matrix<float, 4>& projectionMatrix, util::Vector<float, 3>& cameraPosition)
    {/*
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

      util::Matrix<float, 4> viewProjectionMatrix = projectionMatrix * viewMatrix;
      util::Vector<float, 4> eyeVec = util::Vector<float, 4>(viewMatrix[3][0], viewMatrix[3][1], viewMatrix[3][2], 1.0f);

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

      //collect culling data for static 3D Objects
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
      }

      culledAABB.resize(m_renderGeometry.size() + m_renderAnimatedGeometry.size());
      m_frustumCullingGPU.cullAABB(transformMatrices, bbMin, bbMax, culledAABB);
      //culledAABB.resize(m_renderGeometry.size() + m_renderAnimatedGeometry.size(), 1);

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

      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glDisableVertexAttribArray(RenderShader::SPECIAL0);

      glDisable(GL_BLEND);

      glDepthMask(GL_TRUE);

      m_cameraParameterUBO.unBindBuffer();*/
    }

    void RasterizerRenderManager::addRenderComponent(Sprite* sprite)
    {
      m_spriteRenderer.addRenderComponent(sprite);
    }

    void RasterizerRenderManager::addRenderComponent(sg::BillboardNode *billboardNode)
    {
      m_billboardRenderer.addRenderComponent(billboardNode);
    }

    void RasterizerRenderManager::addRenderComponent(sg::GeoNode *geoNode)
    {
      m_geometryRasterizer.addRenderComponent(geoNode);
    }

    void RasterizerRenderManager::addRenderComponent(sg::AnimatedGeoNode *animatedGeoNode)
    {
      m_geometryRasterizer.addRenderComponent(animatedGeoNode);
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
      m_billboardRenderer.removeRenderComponent(billboardNode);
    }

    void RasterizerRenderManager::removeRenderComponent(Sprite* sprite)
    {
      m_spriteRenderer.removeRenderComponent(sprite);
    }

    void RasterizerRenderManager::removeRenderComponent(sg::GeoNode *geoNode)
    {
      m_geometryRasterizer.removeRenderComponent(geoNode);
    }

    void RasterizerRenderManager::removeRenderComponent(sg::AnimatedGeoNode *animatedGeoNode)
    {
      m_geometryRasterizer.removeRenderComponent(animatedGeoNode);
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
      eventManager->addNewSignal<void (*)(sg::LightNode *node)>(util::EventManager::OnAddLightNode);
      eventManager->addSlotToSignal<RasterizerRenderManager, void (*)(sg::LightNode *node), void (RasterizerRenderManager::*)(sg::LightNode *node)>(this, &RasterizerRenderManager::addRenderComponent, util::EventManager::OnAddLightNode);

      eventManager->addNewSignal<void (*)(sg::ParticleNode *node)>(util::EventManager::OnAddParticleTransmitterNode);
      eventManager->addSlotToSignal<RasterizerRenderManager, void (*)(sg::ParticleNode *node), void (RasterizerRenderManager::*)(sg::ParticleNode *node)>(this, &RasterizerRenderManager::addRenderComponent, util::EventManager::OnAddParticleTransmitterNode);

      eventManager->addNewSignal<void (*)(sg::LightNode *node)>(util::EventManager::OnRemoveLightNode);
      eventManager->addSlotToSignal<RasterizerRenderManager, void (*)(sg::LightNode *node), void (RasterizerRenderManager::*)(sg::LightNode *node)>(this, &RasterizerRenderManager::removeRenderComponent, util::EventManager::OnRemoveLightNode);

      eventManager->addNewSignal<void (*)(sg::ParticleNode *node)>(util::EventManager::OnRemoveParticleTransmitterNode);
      eventManager->addSlotToSignal<RasterizerRenderManager, void (*)(sg::ParticleNode *node), void (RasterizerRenderManager::*)(sg::ParticleNode *node)>(this, &RasterizerRenderManager::removeRenderComponent, util::EventManager::OnRemoveParticleTransmitterNode);
    }
	}
}

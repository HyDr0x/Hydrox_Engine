#include "Renderer/Pipeline/SpriteRenderer.h"

namespace he
{
	namespace renderer
	{
    SpriteRenderer::SpriteRenderer()
    {
    }

    SpriteRenderer::~SpriteRenderer()
    {
      glDeleteBuffers(1, &m_dummyVBO);

      m_opaqueSprites.clear();
      m_transparentSprites.clear();
    }

    const size_t SpriteRenderer::getMaxSpriteLayer() const
    {
      return m_maxLayer;
    }

    void SpriteRenderer::initialize(util::SingletonManager *singletonManager, size_t maxSpriteLayer, util::ResourceHandle spriteShaderHandle)
    {
      m_renderShaderManager = singletonManager->getService<RenderShaderManager>();
      m_textureManager = singletonManager->getService<TextureManager>();

      registerRenderComponentSlots(singletonManager->getService<util::EventManager>());

      m_maxLayer = maxSpriteLayer;

      m_spriteShaderHandle = spriteShaderHandle;

      m_transparentSprites.resize(m_maxLayer);

      glGenBuffers(1, &m_dummyVBO);
      glBindBuffer(GL_ARRAY_BUFFER, m_dummyVBO);
      glBufferData(GL_ARRAY_BUFFER, sizeof(float), nullptr, GL_STATIC_DRAW);
      glVertexAttribPointer(RenderShader::SPECIAL0, 1, GL_FLOAT, GL_FALSE, sizeof(float), NULL);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void SpriteRenderer::render()
    {
      glEnable(GL_BLEND);
	    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      glEnableVertexAttribArray(RenderShader::SPECIAL0);

      glClear(GL_DEPTH_BUFFER_BIT);

      Sprite *renderSprite;
      Texture *renderTexture;
      RenderShader *spriteShader = m_renderShaderManager->getObject(m_spriteShaderHandle);

	    spriteShader->useShader();

	    for(std::list<Sprite*>::iterator spriteIDIterator = m_opaqueSprites.begin(); spriteIDIterator != m_opaqueSprites.end(); spriteIDIterator++)
      {
        renderSprite = (*spriteIDIterator);
        if(renderSprite->getRenderable())
        {
          renderTexture = m_textureManager->getObject(renderSprite->getTextureHandle());

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

      for(unsigned int i = 0; i < m_transparentSprites.size(); i++)//resort all sprites according to their layer if their layer has been changed
      {
        for(std::list<Sprite*>::iterator spriteIDIterator = m_transparentSprites[i].begin(); spriteIDIterator != m_transparentSprites[i].end(); spriteIDIterator++)
        {
          renderSprite = (*spriteIDIterator);
          if(renderSprite->getLayerChanged())
          {
            m_transparentSprites[renderSprite->getLayer()].push_back(*spriteIDIterator);
            renderSprite->spriteSorted();
            spriteIDIterator = m_transparentSprites[i].erase(spriteIDIterator);

            if(spriteIDIterator == m_transparentSprites[i].end())
            {
              break;
            }
          }
        }
      }

      glDepthMask(GL_FALSE);

      glEnable(GL_BLEND);
	    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      for(unsigned int i = 0; i < m_transparentSprites.size(); i++)
      {
        for(std::list<Sprite*>::iterator spriteIDIterator = m_transparentSprites[i].begin(); spriteIDIterator != m_transparentSprites[i].end(); spriteIDIterator++)
        {
          renderSprite = (*spriteIDIterator);
          if(renderSprite->getRenderable())
          {
            renderTexture = m_textureManager->getObject(renderSprite->getTextureHandle());

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
    }

    void SpriteRenderer::addRenderComponent(Sprite* sprite)
    {
      if(sprite->getTransparency())
      {
        m_transparentSprites[sprite->getLayer()].push_back(sprite);
      }
      else
      {
        m_opaqueSprites.push_back(sprite);
      }
    }

    void SpriteRenderer::removeRenderComponent(Sprite* sprite)
    {
      if(sprite->getTransparency())
      {
        m_transparentSprites[sprite->getLayer()].remove(sprite);
      }
      else
      {
        m_opaqueSprites.remove(sprite);
      }
    }

    void SpriteRenderer::registerRenderComponentSlots(util::EventManager *eventManager)
    {
      eventManager->addNewSignal<void (*)(Sprite* sprite)>(util::EventManager::OnAddSprite);
      eventManager->addSlotToSignal<SpriteRenderer, void (*)(Sprite* sprite), void (SpriteRenderer::*)(Sprite* sprite)>(this, &SpriteRenderer::addRenderComponent, util::EventManager::OnAddSprite);

      eventManager->addNewSignal<void (*)(Sprite* sprite)>(util::EventManager::OnRemoveSprite);
      eventManager->addSlotToSignal<SpriteRenderer, void (*)(Sprite* sprite), void (SpriteRenderer::*)(Sprite* sprite)>(this, &SpriteRenderer::removeRenderComponent, util::EventManager::OnRemoveSprite);
    }
	}
}

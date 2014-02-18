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

      m_opaqueSpriteIDs.clear();
      m_transparentSpriteIDs.clear();
    }

    const size_t SpriteRenderer::getMaxSpriteLayer() const
    {
      return m_maxLayer;
    }

    void SpriteRenderer::initialize(util::SingletonManager *singletonManager, size_t maxSpriteLayer, util::ResourceHandle spriteShaderHandle)
    {
      m_renderShaderManager = singletonManager->getService<RenderShaderManager>();
      m_textureManager = singletonManager->getService<TextureManager>();
      m_spriteManager = singletonManager->getService<SpriteManager>();

      m_maxLayer = maxSpriteLayer;

      m_spriteShaderHandle = spriteShaderHandle;

      m_transparentSpriteIDs.resize(m_maxLayer);

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
    }

    void SpriteRenderer::addSprite(util::ResourceHandle spriteID, bool transparent)
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

    void SpriteRenderer::addSprite(const std::list<util::ResourceHandle>& spriteIDList, bool transparent)
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

    void SpriteRenderer::removeSprite(util::ResourceHandle spriteID, bool transparent)
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

    void SpriteRenderer::removeAllSprites()
    {
      m_opaqueSpriteIDs.clear();
      m_transparentSpriteIDs.clear();
    }
	}
}

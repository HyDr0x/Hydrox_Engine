#include "Renderer/Pipeline/SpriteRenderer.h"

#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Signals/EventManager.h>

#include <DataBase/Sprite.h>
#include <DataBase/Texture2D.h>
#include <DataBase/Texture3D.h>

#include "Renderer/Pipeline/RenderOptions.h"

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

    void SpriteRenderer::initialize(util::SingletonManager *singletonManager)
    {
      m_shaderContainer = singletonManager->getService<sh::ShaderContainer>();
      m_textureManager = singletonManager->getService<db::TextureManager>();

      m_spriteShaderHandle = m_shaderContainer->getRenderShaderHandle(sh::ShaderContainer::SPRITE, sh::ShaderSlotFlags(8192));

      registerRenderComponentSlots(singletonManager->getService<util::EventManager>());

      m_maxLayer = singletonManager->getService<RenderOptions>()->max2DLayer;

      m_opaqueSprites.resize(m_maxLayer);
      m_transparentSprites.resize(m_maxLayer);

      glGenBuffers(1, &m_dummyVBO);
      glBindBuffer(GL_ARRAY_BUFFER, m_dummyVBO);
      glBufferData(GL_ARRAY_BUFFER, sizeof(float), nullptr, GL_STATIC_DRAW);
      glVertexAttribPointer(sh::RenderShader::SPECIAL0, 1, GL_FLOAT, GL_FALSE, sizeof(float), NULL);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void SpriteRenderer::render() const
    {
      glEnableVertexAttribArray(sh::RenderShader::SPECIAL0);

      glBindBuffer(GL_ARRAY_BUFFER, m_dummyVBO);

      glClear(GL_DEPTH_BUFFER_BIT);

      const db::Sprite *renderSprite;
      db::Texture2D *renderTexture;
      const sh::RenderShader& spriteShader = m_shaderContainer->getRenderShader(m_spriteShaderHandle);

      spriteShader.useShader();

      for(unsigned int i = 0; i < m_opaqueSprites.size(); i++)
      {
        for(std::list<const db::Sprite*>::const_iterator spriteIDIterator = m_opaqueSprites[i].begin(); spriteIDIterator != m_opaqueSprites[i].end(); spriteIDIterator++)
        {
          renderSprite = (*spriteIDIterator);
          if(renderSprite->getRenderable())
          {
            renderTexture = m_textureManager->getObject(renderSprite->getTextureHandle());

            renderTexture->setTexture(3, 0);

            util::Matrix<float, 3> worldMatrix = renderSprite->getTransformationMatrix();
            util::Matrix<float, 3> textureWorldMatrix = renderSprite->getTexTransformationMatrix();
            float z = renderSprite->getLayer() / (float)m_maxLayer;
            sh::RenderShader::setUniform(0, GL_FLOAT_MAT3, &worldMatrix[0][0]);
            sh::RenderShader::setUniform(1, GL_FLOAT_MAT3, &textureWorldMatrix[0][0]);
            sh::RenderShader::setUniform(2, GL_FLOAT, &z);

            glDrawArrays(GL_POINTS, 0, 1);
          }
        }
      }

      ////////////////////////////////////////////RENDER TRANSPARENT 2D Sprites////////////////////////////////////////////

      glDepthMask(GL_FALSE);

      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      for(unsigned int i = 0; i < m_transparentSprites.size(); i++)
      {
        for(std::list<const db::Sprite*>::const_iterator spriteIDIterator = m_transparentSprites[i].begin(); spriteIDIterator != m_transparentSprites[i].end(); spriteIDIterator++)
        {
          renderSprite = (*spriteIDIterator);
          if(renderSprite->getRenderable())
          {
            renderTexture = m_textureManager->getObject(renderSprite->getTextureHandle());

            renderTexture->setTexture(3, 0);
    
            util::Matrix<float, 3> worldMatrix = renderSprite->getTransformationMatrix();
            util::Matrix<float, 3> textureWorldMatrix = renderSprite->getTexTransformationMatrix();
            float z = renderSprite->getLayer() / (float)m_maxLayer;
            sh::RenderShader::setUniform(0, GL_FLOAT_MAT3, &worldMatrix[0][0]);
            sh::RenderShader::setUniform(1, GL_FLOAT_MAT3, &textureWorldMatrix[0][0]);
            sh::RenderShader::setUniform(2, GL_FLOAT, &z);

            glDrawArrays(GL_POINTS, 0, 1);
          }
        }
      }

      glBindBuffer(GL_ARRAY_BUFFER, 0);

      glDisableVertexAttribArray(sh::RenderShader::SPECIAL0);

      glDisable(GL_BLEND);

      glDepthMask(GL_TRUE);

      spriteShader.useNoShader();
    }

    void SpriteRenderer::addRenderComponent(const db::Sprite* sprite)
    {
      if(sprite->getTransparency())
      {
        m_transparentSprites[sprite->getLayer()].push_back(sprite);
      }
      else
      {
        m_opaqueSprites[sprite->getLayer()].push_back(sprite);
      }
    }

    void SpriteRenderer::removeRenderComponent(const db::Sprite* sprite)
    {
      if(sprite->getTransparency())
      {
        m_transparentSprites[sprite->getLayer()].remove(sprite);
      }
      else
      {
        m_opaqueSprites[sprite->getLayer()].remove(sprite);
      }
    }

    void SpriteRenderer::registerRenderComponentSlots(util::SharedPointer<util::EventManager> eventManager)
    {
      eventManager->addNewSignal<void (*)(const db::Sprite* sprite)>(util::EventManager::OnAddSprite);
      eventManager->addSlotToSignal<SpriteRenderer, void (*)(const db::Sprite* sprite), void (SpriteRenderer::*)(const db::Sprite* sprite)>(this, &SpriteRenderer::addRenderComponent, util::EventManager::OnAddSprite);

      eventManager->addNewSignal<void (*)(const db::Sprite* sprite)>(util::EventManager::OnRemoveSprite);
      eventManager->addSlotToSignal<SpriteRenderer, void (*)(const db::Sprite* sprite), void (SpriteRenderer::*)(const db::Sprite* sprite)>(this, &SpriteRenderer::removeRenderComponent, util::EventManager::OnRemoveSprite);
    }
  }
}

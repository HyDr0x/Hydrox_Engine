#include "Hydrox/Singletons/RenderManager.h"

#include "Hydrox/Graphics/Sprite.h"

namespace he
{
  RenderManager::~RenderManager()
  {
  }

  void RenderManager::initialize(GLfloat aspectRatio, size_t maxSpriteLayer)
  {
    m_aspectRatio = aspectRatio;
    m_maxLayer = maxSpriteLayer;
    m_renderShaderManager = RenderShaderManager::getInstance();
    m_computeShaderManager = ComputeShaderManager::getInstance();
    m_textureManager = TextureManager::getInstance();
    m_billboardManager = BillboardManager::getInstance();
    m_spriteManager = SpriteManager::getInstance();
    m_modelManager = ModelManager::getInstance();
    m_materialManager = MaterialManager::getInstance();

    m_transparentSpriteIDs.resize(m_maxLayer);
  }

  void RenderManager::addSprite(ResourceHandle spriteID, bool transparent)
  {
    if(transparent)
    {
      Sprite *newSprite = m_spriteManager->getObject(spriteID);
      m_transparentSpriteIDs[newSprite->getLayer()].push_back(spriteID);
    }
    else
    {
      m_opaqueSpriteIDs.push_back(spriteID);
    }
  }

  const size_t RenderManager::getMaxSpriteLayer() const
  {
    return m_maxLayer;
  }
}

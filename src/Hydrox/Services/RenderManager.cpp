#include "Hydrox/Services/RenderManager.h"

#include "Hydrox/Graphics/Sprite.h"

namespace he
{
  RenderManager::RenderManager(ModelManager *modelManager, 
                  MaterialManager *materialManager, 
                  ShaderManager *shaderManager, 
                  TextureManager *textureManager,
	                BillboardManager *billboardManager,
                  SpriteManager *spriteManager, GLfloat aspectRatio, size_t maxSpriteLayer) : m_aspectRatio(aspectRatio),
                                                                                              m_maxLayer(maxSpriteLayer),
                                                                                              m_shaderManager(shaderManager),
                                                                                              m_textureManager(textureManager),
                                                                                              m_billboardManager(billboardManager),
                                                                                              m_spriteManager(spriteManager),
                                                                                              m_modelManager(modelManager),
                                                                                              m_materialManager(materialManager)
  { 
    m_transparentSpriteIDs.resize(m_maxLayer);
  }

  RenderManager::~RenderManager()
  {
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
}

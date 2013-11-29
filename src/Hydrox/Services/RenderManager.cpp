#include "Hydrox/Services/RenderManager.h"

#include "Hydrox/Graphics/Sprite.h"

namespace he
{
  RenderManager::RenderManager(ModelManager *modelManager, 
                  MaterialManager *materialManager, 
                  ShaderManager *shaderManager, 
                  TextureManager *textureManager,
	                BillboardManager *billboardManager,
                  SpriteManager *spriteManager, GLfloat aspectRatio) : m_aspectRatio(aspectRatio),
                                                                                                m_shaderManager(shaderManager),
                                                                                                m_textureManager(textureManager),
                                                                                                m_billboardManager(billboardManager),
                                                                                                m_spriteManager(spriteManager),
                                                                                                m_modelManager(modelManager),
                                                                                                m_materialManager(materialManager)
  {
  }

  RenderManager::~RenderManager()
  {
  }

  void RenderManager::addSprite(ResourceHandle spriteID)
  {
    m_spriteIDs.push_back(spriteID);
  }
}

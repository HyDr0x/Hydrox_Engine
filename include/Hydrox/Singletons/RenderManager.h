#ifndef RENDERER_H_
#define RENDERER_H_

#include <list>
#include <vector>

#include <GL/glew.h>

#include "Hydrox/DLLExport.h"

#include "Hydrox/Singletons/CacheManager.hpp"

namespace he
{
  class Scene;
  class Camera;

  class GRAPHICAPI RenderManager
  {
  public:

    RenderManager() {}
	  virtual ~RenderManager() = 0;

    virtual void initialize(ModelManager *modelManager, 
                            MaterialManager *materialManager, 
                            ShaderManager *shaderManager, 
                            TextureManager *textureManager,
	                          BillboardManager *billboardManager,
                            SpriteManager *spriteManager, GLfloat aspectRatio, size_t maxSpriteLayer);

    virtual void addSprite(ResourceHandle spriteID, bool transparent = true);

	  virtual void render(Matrix<float, 4>& viewMatrix, Matrix<float, 4>& projectionMatrix, Vector<float, 3>& cameraPosition, Scene *scene) = 0;

    const size_t getMaxSpriteLayer() const;

  protected:

	  RenderManager(const RenderManager&) : m_maxLayer(0) {}

    std::list<ResourceHandle> m_opaqueSpriteIDs;
    std::vector<std::list<ResourceHandle>> m_transparentSpriteIDs;

    ModelManager *m_modelManager;
	  MaterialManager *m_materialManager;
	  ShaderManager *m_shaderManager;
    TextureManager *m_textureManager;
    BillboardManager *m_billboardManager;
    SpriteManager *m_spriteManager;

    size_t m_maxLayer;
    GLfloat m_aspectRatio;
  };
}

#endif
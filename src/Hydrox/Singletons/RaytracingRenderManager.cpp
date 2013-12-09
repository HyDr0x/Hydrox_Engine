#include "Hydrox/Singletons/RaytracingRenderManager.h"

#include "Hydrox/Graphics/Scene.h"

namespace he
{
  RaytracingRenderManager::~RaytracingRenderManager()
  {
  }

  void RaytracingRenderManager::initialize(ModelManager *modelManager, 
                                           MaterialManager *materialManager, 
                                           ShaderManager *shaderManager, 
                                           TextureManager *textureManager,
	                                         BillboardManager *billboardManager,
                                           SpriteManager *spriteManager, GLfloat aspectRatio, size_t maxSpriteLayer)
  {
    RenderManager::initialize(modelManager, materialManager, shaderManager, textureManager, billboardManager, spriteManager, aspectRatio, maxSpriteLayer);
  }

  void RaytracingRenderManager::render(Matrix<float, 4>& viewMatrix, Matrix<float, 4>& projectionMatrix, Vector<float, 3>& cameraPosition, Scene *scene)
  {
  }
}

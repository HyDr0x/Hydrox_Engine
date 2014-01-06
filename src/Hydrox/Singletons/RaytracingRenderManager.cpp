#include "Hydrox/Singletons/RaytracingRenderManager.h"

#include "Hydrox/Graphics/Scene.h"

namespace he
{
  RaytracingRenderManager::~RaytracingRenderManager()
  {
  }

  void RaytracingRenderManager::initialize(GLfloat aspectRatio, size_t maxSpriteLayer)
  {
    RenderManager::initialize(aspectRatio, maxSpriteLayer);
  }

  void RaytracingRenderManager::render(Matrix<float, 4>& viewMatrix, Matrix<float, 4>& projectionMatrix, Vector<float, 3>& cameraPosition, Scene *scene)
  {
  }
}

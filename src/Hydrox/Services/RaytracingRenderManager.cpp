#include "Hydrox/Services/RaytracingRenderManager.h"

#include "Hydrox/Graphics/Scene.h"

RenderManager* RaytracingRenderManager::getManager(ModelManager *modelManager, 
                                   MaterialManager *materialManager, 
                                   ShaderManager *shaderManager, 
                                   TextureManager *textureManager,
	                                 BillboardManager *billboardManager,
                                   SpriteManager *spriteManager, GLfloat aspectRatio)
{
  static RenderManager *manager = new RaytracingRenderManager(modelManager, materialManager, shaderManager, textureManager, billboardManager, spriteManager, aspectRatio);
  return manager;
}

RaytracingRenderManager::RaytracingRenderManager(ModelManager *modelManager, 
                MaterialManager *materialManager, 
                ShaderManager *shaderManager, 
                TextureManager *textureManager,
	              BillboardManager *billboardManager,
                SpriteManager *spriteManager, GLfloat aspectRatio) : RenderManager(modelManager,
                                                                                    materialManager,
                                                                                    shaderManager, 
                                                                                    textureManager,
                                                                                    billboardManager,
                                                                                    spriteManager,
                                                                                    aspectRatio)
{
}

RaytracingRenderManager::~RaytracingRenderManager()
{
}

void RaytracingRenderManager::initialize()
{

}

void RaytracingRenderManager::render(Matrix<float, 4>& viewMatrix, Matrix<float, 4>& projectionMatrix, Vector<float, 3>& cameraPosition, Scene *scene)
{
}

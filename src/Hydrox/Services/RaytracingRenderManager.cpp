#include "Hydrox/Services/RaytracingRenderManager.h"

#include "Hydrox/Graphics/Scene.h"

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

void RaytracingRenderManager::render(Mat<float, 4>& viewMatrix, Mat<float, 4>& projectionMatrix, Vec<float, 3>& cameraPosition, Scene *scene)
{
}

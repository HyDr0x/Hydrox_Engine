#include "Hydrox/Services/RaytracingRenderManager.h"

#include "Hydrox/Graphics/Scene.h"
#include "Hydrox/Services/Camera.h"


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

void RaytracingRenderManager::render(Camera *camera, Scene *scene)
{
}

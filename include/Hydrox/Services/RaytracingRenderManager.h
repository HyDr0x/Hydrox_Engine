#ifndef RAYTRACINGRENDERER_H_
#define RAYTRACINGRENDERER_H_

#include "Hydrox/DLLExport.h"

#include "Hydrox/Services/ServiceManager.hpp"
#include "Hydrox/Services/RenderManager.h"

class GRAPHICAPI RaytracingRenderManager : public RenderManager
{
public:

	RaytracingRenderManager(ModelManager *modelManager, 
                MaterialManager *materialManager, 
                ShaderManager *shaderManager, 
                TextureManager *textureManager,
	              BillboardManager *billboardManager,
                SpriteManager *spriteManager, GLfloat aspectRatio);
	~RaytracingRenderManager();

	void render(Camera *camera, Scene *scene);

private:

	RaytracingRenderManager(){}
	RaytracingRenderManager(const RaytracingRenderManager&){}
};

#endif
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

	void render(Matrix<float, 4>& viewMatrix, Matrix<float, 4>& projectionMatrix, Vector<float, 3>& cameraPosition, Scene *scene);

private:

	RaytracingRenderManager(){}
	RaytracingRenderManager(const RaytracingRenderManager&){}
};

#endif
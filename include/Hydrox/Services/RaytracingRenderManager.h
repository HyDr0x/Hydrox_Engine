#ifndef RAYTRACINGRENDERER_H_
#define RAYTRACINGRENDERER_H_

#include "Hydrox/DLLExport.h"

#include "Hydrox/Services/Io_service.h"
#include "Hydrox/Services/RenderManager.h"

class GRAPHICAPI RaytracingRenderManager : public RenderManager
{
public:

  static RenderManager* getManager(ModelManager *modelManager, 
                                   MaterialManager *materialManager, 
                                   ShaderManager *shaderManager, 
                                   TextureManager *textureManager,
	                                 BillboardManager *billboardManager,
                                   SpriteManager *spriteManager, GLfloat aspectRatio);

	~RaytracingRenderManager();

  void initialize();

	void render(Matrix<float, 4>& viewMatrix, Matrix<float, 4>& projectionMatrix, Vector<float, 3>& cameraPosition, Scene *scene);

private:

  RaytracingRenderManager(ModelManager *modelManager, 
                MaterialManager *materialManager, 
                ShaderManager *shaderManager, 
                TextureManager *textureManager,
	              BillboardManager *billboardManager,
                SpriteManager *spriteManager, GLfloat aspectRatio);

	RaytracingRenderManager(){}
	RaytracingRenderManager(const RaytracingRenderManager&){}
};

#endif
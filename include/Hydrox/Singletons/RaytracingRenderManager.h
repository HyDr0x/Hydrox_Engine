#ifndef RAYTRACINGRENDERER_H_
#define RAYTRACINGRENDERER_H_

#include "Hydrox/DLLExport.h"

#include "Hydrox/Singletons/RenderManager.h"

namespace he
{
  class GRAPHICAPI RaytracingRenderManager : public RenderManager
  {
  public:

    static void createService(ModelManager *modelManager, 
                              MaterialManager *materialManager, 
                              ShaderManager *shaderManager, 
                              TextureManager *textureManager,
	                            BillboardManager *billboardManager,
                              SpriteManager *spriteManager, GLfloat aspectRatio, size_t maxSpriteLayer);

	  ~RaytracingRenderManager();

    void initialize();

	  void render(Matrix<float, 4>& viewMatrix, Matrix<float, 4>& projectionMatrix, Vector<float, 3>& cameraPosition, Scene *scene);

  private:

    RaytracingRenderManager(ModelManager *modelManager, 
                            MaterialManager *materialManager, 
                            ShaderManager *shaderManager, 
                            TextureManager *textureManager,
	                          BillboardManager *billboardManager,
                            SpriteManager *spriteManager, GLfloat aspectRatio, size_t maxSpriteLayer);

	  RaytracingRenderManager(){}
	  RaytracingRenderManager(const RaytracingRenderManager&){}
  };
}

#endif
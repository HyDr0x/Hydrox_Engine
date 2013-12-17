#ifndef RAYTRACINGRENDERER_H_
#define RAYTRACINGRENDERER_H_

#include "Hydrox/DLLExport.h"

#include "Hydrox/Singletons/RenderManager.h"

namespace he
{
  class GRAPHICAPI RaytracingRenderManager : public RenderManager, public Singleton<RaytracingRenderManager>
  {
  public:

    RaytracingRenderManager(){}
	  ~RaytracingRenderManager();

    void initialize(ModelManager *modelManager, 
                    MaterialManager *materialManager, 
                    RenderShaderManager *renderShaderManager, 
                    ComputeShaderManager *computeShaderManager,
                    TextureManager *textureManager,
	                  BillboardManager *billboardManager,
                    SpriteManager *spriteManager, GLfloat aspectRatio, size_t maxSpriteLayer);

	  void render(Matrix<float, 4>& viewMatrix, Matrix<float, 4>& projectionMatrix, Vector<float, 3>& cameraPosition, Scene *scene);

  private:

	  RaytracingRenderManager(const RaytracingRenderManager&){}
  };
}

#endif
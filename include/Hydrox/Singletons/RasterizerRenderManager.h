#ifndef RASTERIZERRENDERER_H_
#define RASTERIZERRENDERER_H_

#include "Hydrox/DLLExport.h"

#include "Hydrox/Singletons/RenderManager.h"

namespace he
{
  class Shader;

  class GRAPHICAPI RasterizerRenderManager : public RenderManager, public Singleton<RasterizerRenderManager>
  {
  public:

    RasterizerRenderManager(){}
	  ~RasterizerRenderManager();

    void initialize(ModelManager *modelManager, 
                    MaterialManager *materialManager, 
                    RenderShaderManager *renderShaderManager, 
                    ComputeShaderManager *computeShaderManager,
                    TextureManager *textureManager,
	                  BillboardManager *billboardManager,
                    SpriteManager *spriteManager, GLfloat aspectRatio, size_t maxSpriteLayer);

	  void render(Matrix<float, 4>& viewMatrix, Matrix<float, 4>& projectionMatrix, Vector<float, 3>& cameraPosition, Scene *scene);

  private:

	  RasterizerRenderManager(const RasterizerRenderManager&){}

    GLuint m_simpleMeshVAO;
    GLuint m_simpleSkinnedMeshVAO;
    GLuint m_simpleSkinnedTestVAO;

    GLuint m_boneMatricesBuffer;

    GLuint m_dummyVBO;

    ResourceHandle m_billboardHandle;
    ResourceHandle m_spriteHandle;
  };
}

#endif
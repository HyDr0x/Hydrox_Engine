#ifndef RASTERIZERRENDERER_H_
#define RASTERIZERRENDERER_H_

#include "Hydrox/DLLExport.h"

#include "Hydrox/Singletons/RenderManager.h"

namespace he
{
  class Shader;

  class GRAPHICAPI RasterizerRenderManager : public RenderManager
  {
  public:

    static void createService(ModelManager *modelManager, 
                              MaterialManager *materialManager, 
                              ShaderManager *shaderManager, 
                              TextureManager *textureManager,
	                            BillboardManager *billboardManager,
                              SpriteManager *spriteManager, GLfloat aspectRatio, size_t maxSpriteLayer);

	  ~RasterizerRenderManager();

    void initialize();

	  void render(Matrix<float, 4>& viewMatrix, Matrix<float, 4>& projectionMatrix, Vector<float, 3>& cameraPosition, Scene *scene);

  private:

    RasterizerRenderManager(ModelManager *modelManager, 
                  MaterialManager *materialManager, 
                  ShaderManager *shaderManager, 
                  TextureManager *textureManager,
	                BillboardManager *billboardManager,
                  SpriteManager *spriteManager, GLfloat aspectRatio, size_t maxSpriteLayer);

	  RasterizerRenderManager(){}
	  RasterizerRenderManager(const RasterizerRenderManager&){}

    GLuint m_simpleMeshVAO;
    GLuint m_simpleSkinnedMeshVAO;
    GLuint m_simpleSkinnedTestVAO;

    GLuint m_boneMatricesBuffer;

    GLuint m_dummyVBO;

    Shader *m_billboardShader;
    Shader *m_spriteShader;
  };
}

#endif
#ifndef RASTERIZERRENDERER_H_
#define RASTERIZERRENDERER_H_

#include "Hydrox/DLLExport.h"

#include "Hydrox/Services/Io_service.h"
#include "Hydrox/Services/RenderManager.h"

class Shader;

class GRAPHICAPI RasterizerRenderManager : public RenderManager
{
public:

  static RenderManager* getManager(ModelManager *modelManager, 
                                   MaterialManager *materialManager, 
                                   ShaderManager *shaderManager, 
                                   TextureManager *textureManager,
	                                 BillboardManager *billboardManager,
                                   SpriteManager *spriteManager, GLfloat aspectRatio);

	~RasterizerRenderManager();

  void initialize();

	void render(Matrix<float, 4>& viewMatrix, Matrix<float, 4>& projectionMatrix, Vector<float, 3>& cameraPosition, Scene *scene);

private:

  RasterizerRenderManager(ModelManager *modelManager, 
                MaterialManager *materialManager, 
                ShaderManager *shaderManager, 
                TextureManager *textureManager,
	              BillboardManager *billboardManager,
                SpriteManager *spriteManager, GLfloat aspectRatio);

	RasterizerRenderManager(){}
	RasterizerRenderManager(const RasterizerRenderManager&){}

  GLuint m_spriteVBO;

  Shader *m_billboardShader;
  Shader *m_spriteShader;
};

#endif
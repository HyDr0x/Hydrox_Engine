#ifndef RASTERIZERRENDERER_H_
#define RASTERIZERRENDERER_H_

#include "Hydrox/DLLExport.h"

#include "Hydrox/Services/ServiceManager.hpp"
#include "Hydrox/Services/RenderManager.h"

class Shader;

class GRAPHICAPI RasterizerRenderManager : public RenderManager
{
public:

	RasterizerRenderManager(ModelManager *modelManager, 
                MaterialManager *materialManager, 
                ShaderManager *shaderManager, 
                TextureManager *textureManager,
	              BillboardManager *billboardManager,
                SpriteManager *spriteManager, GLfloat aspectRatio);
	~RasterizerRenderManager();

	void render(Camera *camera, Scene *scene);

private:

	RasterizerRenderManager(){}
	RasterizerRenderManager(const RasterizerRenderManager&){}

  GLuint m_spriteVBO;

  Shader *m_billboardShader;
  Shader *m_spriteShader;
};

#endif
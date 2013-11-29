#ifndef RENDERER_H_
#define RENDERER_H_

#include <list>

#include <GL/glew.h>

#include "Hydrox/DLLExport.h"

#include "Hydrox/Services/Io_service.h"

#include "Hydrox/Services/CacheManager.hpp"

class Sprite;
class Scene;
class Camera;

class GRAPHICAPI RenderManager : public Io_service
{
public:

	virtual ~RenderManager();

  virtual void initialize() = 0;

  virtual void addSprite(ResourceHandle spriteID);
	virtual void render(Matrix<float, 4>& viewMatrix, Matrix<float, 4>& projectionMatrix, Vector<float, 3>& cameraPosition, Scene *scene) = 0;

protected:

  RenderManager(ModelManager *modelManager, 
                MaterialManager *materialManager, 
                ShaderManager *shaderManager, 
                TextureManager *textureManager,
	              BillboardManager *billboardManager,
                SpriteManager *spriteManager, GLfloat aspectRatio);

  RenderManager(){}
	RenderManager(const RenderManager&){}

  ModelManager    *m_modelManager;
	MaterialManager *m_materialManager;
	ShaderManager   *m_shaderManager;
  TextureManager  *m_textureManager;
  BillboardManager *m_billboardManager;
  SpriteManager   *m_spriteManager;

  std::list<ResourceHandle> m_spriteIDs;

  GLfloat m_aspectRatio;
};

#endif
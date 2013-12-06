#ifndef GRAPHIC_ENGINE_H_
#define GRAPHIC_ENGINE_H_

#include <iostream>

#include <GL/glew.h>
#include <IL/il.h>

#include "Hydrox/DLLExport.h"

#include "Hydrox/Utility/Math/Math.hpp"

#include "Hydrox/Services/CacheManager.hpp"

namespace he
{
  class Scene;

  class ServiceManager;
  class RenderManager;
  class RasterizerRenderManager;
  class RaytracingRenderManager;
  class EventManager;
  class DebugLogManager;

  class GRAPHICAPI GraphicEngine
  {
  public:
	
	  GraphicEngine();
	  virtual ~GraphicEngine();

    Scene* getScene();

    void setRenderOptions(unsigned int width, unsigned int height, float aspectRatio);

	  void setClearColor(Vector<float, 4> color);

    void registerServices(ServiceManager *serviceManager);

	  void initialize(std::string vfxPath, std::string texPath, std::string modelPath, std::string materialPath, std::string worldRootNodeName, unsigned int width, unsigned int height, float aspectRatio, bool debugMode);
	  void update(Vector<float, 3>& cameraPosition, float currentTime);
	  void draw(Matrix<float, 4>& viewMatrix, Matrix<float, 4>& projectionMatrix, Vector<float, 3>& cameraPosition);

  private:

    Scene *m_scene;

	  RenderManager *m_renderManager;
	  EventManager *m_eventManager;
	  ModelManager *m_modelManager;
	  MaterialManager *m_materialManager;
	  ShaderManager *m_shaderManager;
    TextureManager *m_textureManager;
    BillboardManager *m_billboardManager;
    SpriteManager *m_spriteManager;
	  DebugLogManager *m_debugLogManager;

    float m_aspectRatio;
    unsigned int m_width;
    unsigned int m_height;

    bool m_initialized;
  };
}

#endif
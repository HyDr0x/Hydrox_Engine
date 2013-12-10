#include "Hydrox/Graphics/GraphicEngine.h"

#include "Hydrox/Graphics/Scene.h"

#include "Hydrox/Singletons/Singleton.hpp"
#include "Hydrox/Singletons/RenderManager.h"
#include "Hydrox/Singletons/RasterizerRenderManager.h"
#include "Hydrox/Singletons/RaytracingRenderManager.h"
#include "Hydrox/Singletons/Signals/EventManager.h"
#include "Hydrox/Singletons/DebugLogManager.h"

#include "Hydrox/Utility/Tree/TransformNode.h"

namespace he
{
  GraphicEngine::GraphicEngine()
  {
    m_initialized = false;

    m_scene = nullptr;

	  m_modelManager = nullptr;
	  m_textureManager = nullptr;
	  m_shaderManager = nullptr;
	  m_renderManager = nullptr;
    m_materialManager = nullptr;
	  m_eventManager = nullptr;
    m_debugLogManager = nullptr;
  }

  GraphicEngine::~GraphicEngine()
  {
    if(m_initialized)
    {
      delete m_scene;
      RasterizerRenderManager::deleteInstance();
      DebugLogManager::deleteInstance();
      EventManager::deleteInstance();
      ModelManager::deleteInstance();
      MaterialManager::deleteInstance();
      TextureManager::deleteInstance();
      ShaderManager::deleteInstance();
    }
  }

  Scene* GraphicEngine::getScene()
  {
    return m_scene;
  }

  void GraphicEngine::setRenderOptions(unsigned int width, unsigned int height, float aspectRatio)
  {
    m_width = width;
    m_height = height;
    m_aspectRatio = aspectRatio;
  }

  void GraphicEngine::setClearColor(Vector<float, 4> color)
  {
	  glClearColor(color[0], color[1], color[2], color[3]);
  }

  void GraphicEngine::resizeRenderWindow(unsigned int width, unsigned int height)
  {
    glViewport(0, 0, width, height);
  }
 
  void GraphicEngine::initialize(std::string vfxPath, std::string texPath, std::string modelPath, std::string materialPath, unsigned int width, unsigned int height, std::string worldRootNodeName, size_t spriteLayer)
  {
    m_initialized = true;

    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    setRenderOptions(width, height, aspectRatio);

    ilInit();

	  GLenum err = glewInit();
	  if(err != GLEW_OK)
	  {
		  fprintf(stderr,"GLEW error: %s \n", glewGetErrorString(err));
		  //exit(EXIT_FAILURE);
	  }

	  glEnable(GL_DEPTH_TEST);
	  glDepthMask(GL_TRUE);
	  glDepthFunc(GL_GREATER);

	  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	  glClearDepth(0.0f);
	  //glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	  //create Manager
    m_modelManager = ModelManager::getInstance();
    m_materialManager = MaterialManager::getInstance();
    m_shaderManager = ShaderManager::getInstance();
	  m_textureManager = TextureManager::getInstance();
    m_billboardManager = BillboardManager::getInstance();
    m_spriteManager = SpriteManager::getInstance();
	  m_eventManager = EventManager::getInstance();
    m_debugLogManager = DebugLogManager::getInstance();
	  m_renderManager = RasterizerRenderManager::getInstance();

    //initialize the manager
    m_modelManager->initialize(modelPath);
    m_materialManager->initialize(materialPath);
    m_shaderManager->initialize(vfxPath);
    m_textureManager->initialize(texPath);
    m_billboardManager->initialize(texPath);
    m_spriteManager->initialize(texPath);
    m_renderManager->initialize(m_modelManager, m_materialManager, m_shaderManager, m_textureManager, m_billboardManager, m_spriteManager, m_aspectRatio, spriteLayer);

    m_scene = new Scene(new TransformNode(Matrix<float, 4>::identity(), worldRootNodeName), Vector<float, 3>::identity());

    m_debugLogManager->gatherSystemInformation();
    m_debugLogManager->printSystemInformation();

    if(m_debugLogManager->getMajorOpenGLVersion() < 4 || m_debugLogManager->getMinorOpenGLVersion() < 3)
    {
      std::cout << "Error!" << std::endl;
      std::cout << "OpenGL version too old, version 4.3 or higher needed." << std::endl;
    }

    assert(m_debugLogManager->getMajorOpenGLVersion() >= 4 && m_debugLogManager->getMinorOpenGLVersion() >= 3);
  }

  void GraphicEngine::update(Vector<float, 3>& cameraPosition, float currentTime)
  {
    m_scene->updateCaches(cameraPosition, currentTime);
  }

  void GraphicEngine::draw(Matrix<float, 4>& viewMatrix, Matrix<float, 4>& projectionMatrix, Vector<float, 3>& cameraPosition)
  {
    m_renderManager->render(viewMatrix, projectionMatrix, cameraPosition, m_scene);
  }
}
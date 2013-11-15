#include "Hydrox/Graphics/GraphicEngine.h"

#include "Hydrox/Graphics/Scene.h"

#include "Hydrox/Services/ServiceManager.hpp"
#include "Hydrox/Services/RenderManager.h"
#include "Hydrox/Services/RasterizerRenderManager.h"
#include "Hydrox/Services/RaytracingRenderManager.h"
#include "Hydrox/Services/Signals/EventManager.h"

#include "Hydrox/Utility/Tree/TransformNode.h"


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
}

GraphicEngine::~GraphicEngine()
{
  if(m_initialized)
  {
    delete m_scene;
	  delete m_modelManager;
	  delete m_textureManager;
	  delete m_shaderManager;
	  delete m_renderManager;
    delete m_materialManager;
	  delete m_eventManager;
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

void GraphicEngine::registerServices(ServiceManager *serviceManager)
{
	serviceManager->addService(m_modelManager);
  serviceManager->addService(m_materialManager);
  serviceManager->addService(m_shaderManager);
	serviceManager->addService(m_textureManager);
  serviceManager->addService(m_billboardManager);
	serviceManager->addService(m_spriteManager);

  serviceManager->addService(m_renderManager);
	serviceManager->addService(m_eventManager);
}
 
void GraphicEngine::initialize(std::string vfxPath, std::string texPath, std::string modelPath, std::string materialPath, std::string worldRootNodeName, unsigned int width, unsigned int height, float aspectRatio)
{
  m_initialized = true;

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
  m_modelManager = new ModelManager(modelPath);
  m_materialManager = new MaterialManager(materialPath);
  m_shaderManager = new CacheManager<Shader>(vfxPath);
	m_textureManager = new TextureManager(texPath);
  m_billboardManager = new BillboardManager(texPath);
  m_spriteManager = new SpriteManager(texPath);
	m_renderManager = new RasterizerRenderManager(m_modelManager, m_materialManager, m_shaderManager, m_textureManager, m_billboardManager, m_spriteManager, m_aspectRatio);
	m_eventManager = new EventManager();
  
  m_scene = new Scene(new TransformNode(Matrix<float, 4>::identity(), worldRootNodeName), Vector<float, 3>::identity());
}

void GraphicEngine::update(Vector<float, 3>& cameraPosition)
{
  m_scene->updateCaches(cameraPosition);
}

void GraphicEngine::draw(Matrix<float, 4>& viewMatrix, Matrix<float, 4>& projectionMatrix, Vector<float, 3>& cameraPosition)
{
  m_renderManager->render(viewMatrix, projectionMatrix, cameraPosition, m_scene);
}

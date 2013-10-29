#include "Hydrox/Services/RasterizerRenderManager.h"

#include "Hydrox/Graphics/Scene.h"
#include "Hydrox/Services/Camera.h"

#include "Hydrox/Graphics/Mesh.h"
#include "Hydrox/Graphics/Material.h"
#include "Hydrox/Graphics/Shader.h"
#include "Hydrox/Graphics/Texture.h"
#include "Hydrox/Graphics/Billboard.h"
#include "Hydrox/Graphics/Sprite.h"

#include "Hydrox/Utility/Tree/GeoNode.h"
#include "Hydrox/Utility/Tree/LODNode.h"
#include "Hydrox/Utility/Tree/TransformNode.h"
#include "Hydrox/Utility/Tree/BillboardNode.h"
#include "Hydrox/Utility/Tree/ParticleNode.h"
#include "Hydrox/Utility/Tree/LightNode.h"


RasterizerRenderManager::RasterizerRenderManager(ModelManager *modelManager, 
                MaterialManager *materialManager, 
                ShaderManager *shaderManager, 
                TextureManager *textureManager,
	              BillboardManager *billboardManager,
                SpriteManager *spriteManager, GLfloat aspectRatio) : RenderManager(modelManager,
                                                                                    materialManager,
                                                                                    shaderManager, 
                                                                                    textureManager,
                                                                                    billboardManager,
                                                                                    spriteManager,
                                                                                    aspectRatio)
{
  float placeholder = 0;
  glGenBuffers(1, &m_spriteVBO);

  glBindBuffer(GL_ARRAY_BUFFER, m_spriteVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(placeholder), &placeholder, GL_STATIC_DRAW);
  glVertexAttribPointer(Shader::SPECIAL0, 1, GL_FLOAT, GL_FALSE, sizeof(placeholder), NULL);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glVertexAttribFormat(Shader::POSITION, 3, GL_FLOAT, GL_FALSE, 0);
  glVertexAttribBinding(Shader::POSITION, 1);
  glVertexAttribFormat(Shader::TEXTURE0, 2, GL_FLOAT, GL_FALSE, sizeof(Vec<float, 3>));
  glVertexAttribBinding(Shader::TEXTURE0, 1);

  std::string shaderPath = shaderManager->getPath();

  std::string billboardVertName = "Shader/billboardShader.vert";
  std::string billboardFragName = "Shader/billboardShader.frag";
  std::string billboardGeomName = "Shader/billboardShader.geom";

  std::string spriteVertName = "Shader/spriteShader.vert";
  std::string spriteFragName = "Shader/spriteShader.frag";
  std::string spriteGeomName = "Shader/spriteShader.geom";

  m_billboardShader = new Shader((shaderPath + billboardVertName).c_str(), (shaderPath + billboardFragName).c_str(), (shaderPath + billboardGeomName).c_str(), nullptr, nullptr, nullptr);
  m_spriteShader = new Shader((shaderPath + spriteVertName).c_str(), (shaderPath + spriteFragName).c_str(), (shaderPath + spriteGeomName).c_str(), nullptr, nullptr, nullptr);
}

RasterizerRenderManager::~RasterizerRenderManager()
{
  glDeleteBuffers(1, &m_spriteVBO);
  delete m_billboardShader;
  delete m_spriteShader;
}

void RasterizerRenderManager::render(Camera *camera, Scene *scene)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	////////////////////////////////RENDER 3D Objects////////////////////////////////////////////
  Mesh *renderMesh;
  Material *renderMaterial;
  Shader *renderShader;
  Texture *renderTexture;

  Mat<float, 4> worldViewProjectionMatrix;

  const std::list<GeoNode*> renderGeometryList = scene->getMeshes();

  glEnableVertexAttribArray(Shader::POSITION);
  glEnableVertexAttribArray(Shader::TEXTURE0);

  for(std::list<GeoNode*>::const_iterator geometryIterator = renderGeometryList.begin(); geometryIterator != renderGeometryList.end(); geometryIterator++)//Render 3D Objects
  {
    if((*geometryIterator)->getRenderable())
    {
      renderMesh = m_modelManager->getObject((*geometryIterator)->getMeshIndex());
      renderMaterial = m_materialManager->getObject(renderMesh->getMaterial());
      renderShader = m_shaderManager->getObject(renderMaterial->getShader());
      //renderTexture = m_textureManager->getObject(renderMaterial->getTexture(Material::DIFFUSETEX, 0));

      renderShader->useShader();

      worldViewProjectionMatrix = camera->getViewProjectionMatrix() * (*geometryIterator)->getTransformationMatrix();
      renderShader->setUniform(4, GL_FLOAT_MAT4, &(worldViewProjectionMatrix[0][0]));

      /*renderShader->setTexture(0, 0);
      renderTexture->setTexture(0);*/

      renderMesh->render(1);
    }
  }

	glBindVertexBuffer(0, 0, 0, 0);//restore renderstates to avoid problems with attribute slots in the shader
  glDisableVertexAttribArray(Shader::POSITION);
  glDisableVertexAttribArray(Shader::TEXTURE0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);


	////////////////////////////////RENDER BILLBOARDS////////////////////////////////////////////

  const std::list<BillboardNode*> renderBillboardList = scene->getBillboards();

  Billboard *renderBillboard;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnableVertexAttribArray(Shader::SPECIAL0);
 
	m_billboardShader->useShader();

  Mat<float, 4> projectionMatrix = camera->getProjectionMatrix();
	Mat<float, 4> viewMatrix = camera->getViewMatrix();

  m_billboardShader->setUniform(1, GL_FLOAT_MAT4, &viewMatrix[0][0]);
  m_billboardShader->setUniform(2, GL_FLOAT_MAT4, &projectionMatrix[0][0]);

	for(std::list<BillboardNode*>::const_iterator billboarditerator = renderBillboardList.begin(); billboarditerator != renderBillboardList.end(); billboarditerator++)
	{
    renderBillboard = m_billboardManager->getObject((*billboarditerator)->getBillboardIndex());
    renderTexture = m_textureManager->getObject(renderBillboard->getTextureID());

    renderTexture->setTexture(0);
		m_billboardShader->setTexture(6, 0);

    Mat<float, 4> worldMatrix = (*billboarditerator)->getTransformationMatrix();
		Mat<float, 3> tmpTexTrfMatrix = renderBillboard->getTexTransformationMatrix();
    Vec<float, 2> scale = renderBillboard->getScale();
		Vec<float, 3> translate = renderBillboard->getPosition();
    m_billboardShader->setUniform(0, GL_FLOAT_MAT4, &worldMatrix[0][0]);
		m_billboardShader->setUniform(3, GL_FLOAT_MAT3, &tmpTexTrfMatrix[0][0]);
		m_billboardShader->setUniform(4, GL_FLOAT_VEC2, &scale[0]);
		m_billboardShader->setUniform(5, GL_FLOAT_VEC3, &translate[0]);

    glBindBuffer(GL_VERTEX_ARRAY, m_spriteVBO);

    glDrawArrays(GL_POINTS, 0, 1);
	}


	////////////////////////////////RENDER 2D Sprites////////////////////////////////////////////

  Sprite *renderSprite;

	glDisable(GL_DEPTH_TEST);
	m_spriteShader->useShader();

	for(std::list<ResourceHandle>::iterator spriteIDIterator = m_spriteIDs.begin(); spriteIDIterator != m_spriteIDs.end(); spriteIDIterator++)
	{
    renderSprite = m_spriteManager->getObject(*spriteIDIterator);
    renderTexture = m_textureManager->getObject(renderSprite->getTextureID());

    renderTexture->setTexture(0);
		m_spriteShader->setTexture(2, 0);
		
		Mat<float,3> worldMatrix = renderSprite->getTransformationMatrix() * Mat<float,3>(1.0f / m_aspectRatio,0,0, 0,1,0, 0,0,1);
		Mat<float,3> textureWorldMatrix = renderSprite->getTexTransformationMatrix();
		m_spriteShader->setUniform(0, GL_FLOAT_MAT3, &worldMatrix[0][0]);
		m_spriteShader->setUniform(1, GL_FLOAT_MAT3, &textureWorldMatrix[0][0]);

    glBindBuffer(GL_VERTEX_ARRAY, m_spriteVBO);
		glDrawArrays(GL_POINTS, 0, 1);
	}

  glDisableVertexAttribArray(Shader::SPECIAL0);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}

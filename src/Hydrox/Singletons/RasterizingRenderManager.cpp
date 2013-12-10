#include "Hydrox/Singletons/RasterizerRenderManager.h"

#include "Hydrox/Graphics/Scene.h"

#include "Hydrox/Graphics/Mesh.h"
#include "Hydrox/Graphics/Material.h"
#include "Hydrox/Graphics/Shader.h"
#include "Hydrox/Graphics/Texture.h"
#include "Hydrox/Graphics/Billboard.h"
#include "Hydrox/Graphics/Sprite.h"

#include "Hydrox/Utility/Tree/AnimatedGeoNode.h"
#include "Hydrox/Utility/Tree/GeoNode.h"
#include "Hydrox/Utility/Tree/LODNode.h"
#include "Hydrox/Utility/Tree/TransformNode.h"
#include "Hydrox/Utility/Tree/BillboardNode.h"
#include "Hydrox/Utility/Tree/ParticleNode.h"
#include "Hydrox/Utility/Tree/LightNode.h"

namespace he
{
  RasterizerRenderManager::~RasterizerRenderManager()
  {
    glDeleteBuffers(1, &m_boneMatricesBuffer);
    glDeleteBuffers(1, &m_dummyVBO);
    glDeleteVertexArrays(1, &m_simpleMeshVAO);
    glDeleteVertexArrays(1, &m_simpleSkinnedMeshVAO);
    glDeleteVertexArrays(1, &m_simpleSkinnedTestVAO);
    delete m_billboardShader;
    delete m_spriteShader;
  }

  void RasterizerRenderManager::initialize(ModelManager *modelManager, 
                                           MaterialManager *materialManager, 
                                           ShaderManager *shaderManager, 
                                           TextureManager *textureManager,
	                                         BillboardManager *billboardManager,
                                           SpriteManager *spriteManager, GLfloat aspectRatio, size_t maxSpriteLayer)
  {
    RenderManager::initialize(modelManager, materialManager, shaderManager, textureManager, billboardManager, spriteManager, aspectRatio, maxSpriteLayer);

    glGenBuffers(1, &m_dummyVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_dummyVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float), nullptr, GL_STATIC_DRAW);
    glVertexAttribPointer(Shader::SPECIAL0, 1, GL_FLOAT, GL_FALSE, sizeof(float), NULL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glGenBuffers(1, &m_boneMatricesBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_boneMatricesBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Matrix<float, 4>) * 64, nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenVertexArrays(1, &m_simpleMeshVAO);
    glBindVertexArray(m_simpleMeshVAO);
    glVertexAttribFormat(Shader::POSITION, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribFormat(Shader::TEXTURE0, 2, GL_FLOAT, GL_FALSE, sizeof(Vector<float, 3>));
    //glVertexAttribFormat(Shader::NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vector<float, 3>));

    glVertexAttribBinding(Shader::POSITION, 0);
    glVertexAttribBinding(Shader::TEXTURE0, 0);
    //glVertexAttribBinding(Shader::NORMAL, 0);

    glEnableVertexAttribArray(Shader::POSITION);
    glEnableVertexAttribArray(Shader::TEXTURE0);
    //glEnableVertexAttribArray(Shader::NORMAL);
    glBindVertexArray(0);

    glGenVertexArrays(1, &m_simpleSkinnedMeshVAO);
    glBindVertexArray(m_simpleSkinnedMeshVAO);
    glVertexAttribFormat(Shader::POSITION, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribFormat(Shader::TEXTURE0, 2, GL_FLOAT, GL_FALSE, sizeof(Vector<float, 3>));
    glVertexAttribFormat(Shader::NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vector<float, 3>) + sizeof(Vector<float, 2>));
    glVertexAttribFormat(Shader::BINORMAL, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(Vector<float, 3>) + sizeof(Vector<float, 2>));
    glVertexAttribFormat(Shader::BONEWEIGHTS, 4, GL_FLOAT, GL_FALSE, 3 * sizeof(Vector<float, 3>) + sizeof(Vector<float, 2>));
    glVertexAttribFormat(Shader::BONEINDICES, 4, GL_FLOAT, GL_FALSE, sizeof(Vector<float, 4>) + 3 * sizeof(Vector<float, 3>) + sizeof(Vector<float, 2>));

    glVertexAttribBinding(Shader::POSITION, 0);
    glVertexAttribBinding(Shader::TEXTURE0, 0);
    glVertexAttribBinding(Shader::NORMAL, 0);
    glVertexAttribBinding(Shader::BINORMAL, 0);
    glVertexAttribBinding(Shader::BONEWEIGHTS, 0);
    glVertexAttribBinding(Shader::BONEINDICES, 0);

    glEnableVertexAttribArray(Shader::POSITION);
    glEnableVertexAttribArray(Shader::TEXTURE0);
    glEnableVertexAttribArray(Shader::NORMAL);
    glEnableVertexAttribArray(Shader::BINORMAL);
    glEnableVertexAttribArray(Shader::BONEWEIGHTS);
    glEnableVertexAttribArray(Shader::BONEINDICES);
    glBindVertexArray(0);

    glGenVertexArrays(1, &m_simpleSkinnedTestVAO);
    glBindVertexArray(m_simpleSkinnedTestVAO);
    glVertexAttribFormat(Shader::POSITION, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribFormat(Shader::NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vector<float, 3>));
    glVertexAttribFormat(Shader::BONEWEIGHTS, 4, GL_FLOAT, GL_FALSE, 2 * sizeof(Vector<float, 3>));
    glVertexAttribFormat(Shader::BONEINDICES, 4, GL_FLOAT, GL_FALSE, sizeof(Vector<float, 4>) + 2 * sizeof(Vector<float, 3>));

    glVertexAttribBinding(Shader::POSITION, 0);
    glVertexAttribBinding(Shader::NORMAL, 0);
    glVertexAttribBinding(Shader::BONEWEIGHTS, 0);
    glVertexAttribBinding(Shader::BONEINDICES, 0);

    glEnableVertexAttribArray(Shader::POSITION);
    glEnableVertexAttribArray(Shader::NORMAL);
    glEnableVertexAttribArray(Shader::BONEWEIGHTS);
    glEnableVertexAttribArray(Shader::BONEINDICES);
    glBindVertexArray(0);

    std::string shaderPath = m_shaderManager->getPath();

    std::string billboardVertName = "Shader/billboardShader.vert";
    std::string billboardFragName = "Shader/billboardShader.frag";
    std::string billboardGeomName = "Shader/billboardShader.geom";

    std::string spriteVertName = "Shader/spriteShader.vert";
    std::string spriteFragName = "Shader/spriteShader.frag";
    std::string spriteGeomName = "Shader/spriteShader.geom";

    m_billboardShader = new Shader((shaderPath + billboardVertName).c_str(), (shaderPath + billboardFragName).c_str(), (shaderPath + billboardGeomName).c_str(), nullptr, nullptr, nullptr);
    m_spriteShader = new Shader((shaderPath + spriteVertName).c_str(), (shaderPath + spriteFragName).c_str(), (shaderPath + spriteGeomName).c_str(), nullptr, nullptr, nullptr);
  }

  void RasterizerRenderManager::render(Matrix<float, 4>& viewMatrix, Matrix<float, 4>& projectionMatrix, Vector<float, 3>& cameraPosition, Scene *scene)
  {
	  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	  ////////////////////////////////RENDER 3D Objects////////////////////////////////////////////
    Mesh *renderMesh;
    Material *renderMaterial;
    Shader *renderShader;
    Texture *renderTexture;

    Matrix<float, 4> viewProjectionMatrix = projectionMatrix * viewMatrix;
    Matrix<float, 4> worldViewProjectionMatrix;
    
    glBindVertexArray(m_simpleMeshVAO);

    const std::list<GeoNode*> renderGeometryList = scene->getMeshes();

    for(std::list<GeoNode*>::const_iterator geometryIterator = renderGeometryList.begin(); geometryIterator != renderGeometryList.end(); geometryIterator++)//Render 3D Objects
    {
      if((*geometryIterator)->getRenderable())
      {
        renderMesh = m_modelManager->getObject((*geometryIterator)->getMeshIndex());
        renderMaterial = m_materialManager->getObject(renderMesh->getMaterial());
        renderShader = m_shaderManager->getObject(renderMaterial->getShader());
        //renderTexture = m_textureManager->getObject(renderMaterial->getTexture(Material::DIFFUSETEX, 0));

        renderShader->useShader();

        worldViewProjectionMatrix = viewProjectionMatrix * (*geometryIterator)->getTransformationMatrix();
        renderShader->setUniform(4, GL_FLOAT_MAT4, &(worldViewProjectionMatrix[0][0]));

        //renderShader->setTexture(0, 0);
        //renderTexture->setTexture(0);

        renderMesh->render(0);
      }
    }

    glBindVertexArray(m_simpleSkinnedMeshVAO);

    const std::list<AnimatedGeoNode*> renderAnimatedGeometryList = scene->getAnimatedMeshes();

    for(std::list<AnimatedGeoNode*>::const_iterator animatedGeometryIterator = renderAnimatedGeometryList.begin(); animatedGeometryIterator != renderAnimatedGeometryList.end(); animatedGeometryIterator++)//Render 3D Objects
    {
      if((*animatedGeometryIterator)->getRenderable())
      {
        renderMesh = m_modelManager->getObject((*animatedGeometryIterator)->getMeshIndex());
        renderMaterial = m_materialManager->getObject(renderMesh->getMaterial());
        renderShader = m_shaderManager->getObject(renderMaterial->getShader());
        //renderTexture = m_textureManager->getObject(renderMaterial->getTexture(Material::DIFFUSETEX, 0));

        renderShader->useShader();

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_boneMatricesBuffer);
        std::vector<Matrix<float, 4>> skinningMatrices = (*animatedGeometryIterator)->getSkinningMatrices();

        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(Matrix<float, 4>) * skinningMatrices.size(), &(skinningMatrices[0][0][0]));
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        renderShader->setUniform(4, GL_FLOAT_MAT4, &(viewProjectionMatrix[0][0]));
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_boneMatricesBuffer); 
        //renderShader->setTexture(0, 0);
        //renderTexture->setTexture(0);

        renderMesh->render(0);
      }
    }
    glBindVertexArray(0);

	  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	  ////////////////////////////////RENDER BILLBOARDS////////////////////////////////////////////

    const std::list<BillboardNode*> renderBillboardList = scene->getBillboards();

    Billboard *renderBillboard;

	  glEnable(GL_BLEND);
	  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnableVertexAttribArray(Shader::SPECIAL0);
 
	  m_billboardShader->useShader();

    m_billboardShader->setUniform(1, GL_FLOAT_MAT4, &viewMatrix[0][0]);
    m_billboardShader->setUniform(2, GL_FLOAT_MAT4, &projectionMatrix[0][0]);

	  for(std::list<BillboardNode*>::const_iterator billboarditerator = renderBillboardList.begin(); billboarditerator != renderBillboardList.end(); billboarditerator++)
	  {
      renderBillboard = m_billboardManager->getObject((*billboarditerator)->getBillboardIndex());
      if(renderBillboard->getRenderable())
      {
        renderTexture = m_textureManager->getObject(renderBillboard->getTextureID());

        renderTexture->setTexture(0);
		    m_billboardShader->setTexture(6, 0);

        Matrix<float, 4> worldMatrix = (*billboarditerator)->getTransformationMatrix();
		    Matrix<float, 3> tmpTexTrfMatrix = renderBillboard->getTexTransformationMatrix();
        Vector<float, 2> scale = renderBillboard->getScale();
		    Vector<float, 3> translate = renderBillboard->getPosition();
        m_billboardShader->setUniform(0, GL_FLOAT_MAT4, &worldMatrix[0][0]);
		    m_billboardShader->setUniform(3, GL_FLOAT_MAT3, &tmpTexTrfMatrix[0][0]);
		    m_billboardShader->setUniform(4, GL_FLOAT_VEC2, &scale[0]);
		    m_billboardShader->setUniform(5, GL_FLOAT_VEC3, &translate[0]);

        glBindBuffer(GL_ARRAY_BUFFER, m_dummyVBO);
        glDrawArrays(GL_POINTS, 0, 1);
      }
	  }
    
    glDisable(GL_BLEND);

	  ////////////////////////////////RENDER OPAQUE 2D Sprites////////////////////////////////////////////

    glClear(GL_DEPTH_BUFFER_BIT);

    Sprite *renderSprite;

	  m_spriteShader->useShader();

	  for(std::list<ResourceHandle>::iterator spriteIDIterator = m_opaqueSpriteIDs.begin(); spriteIDIterator != m_opaqueSpriteIDs.end(); spriteIDIterator++)
    {
      renderSprite = m_spriteManager->getObject(*spriteIDIterator);
      if(renderSprite->getRenderable())
      {
        renderTexture = m_textureManager->getObject(renderSprite->getTextureID());

        renderTexture->setTexture(0);

		    m_spriteShader->setTexture(3, 0);
		
		    Matrix<float, 3> worldMatrix = renderSprite->getTransformationMatrix()/* * Matrix<float, 3>(1.0f / m_aspectRatio,0,0, 0,1,0, 0,0,1)*/;
		    Matrix<float, 3> textureWorldMatrix = renderSprite->getTexTransformationMatrix();
        float z = renderSprite->getLayer() / (const float)m_maxLayer;
		    m_spriteShader->setUniform(0, GL_FLOAT_MAT3, &worldMatrix[0][0]);
		    m_spriteShader->setUniform(1, GL_FLOAT_MAT3, &textureWorldMatrix[0][0]);
        m_spriteShader->setUniform(2, GL_FLOAT, &z);

        glBindBuffer(GL_ARRAY_BUFFER, m_dummyVBO);
		    glDrawArrays(GL_POINTS, 0, 1);
      }
	  }

    ////////////////////////////////RENDER TRANSPARENT 2D Sprites////////////////////////////////////////////

    for(unsigned int i = 0; i < m_transparentSpriteIDs.size(); i++)//sort all sprites according to their layer
    {
      for(std::list<ResourceHandle>::iterator spriteIDIterator = m_transparentSpriteIDs[i].begin(); spriteIDIterator != m_transparentSpriteIDs[i].end(); spriteIDIterator++)
      {
        renderSprite = m_spriteManager->getObject(*spriteIDIterator);
        if(renderSprite->getLayerChanged())
        {
          m_transparentSpriteIDs[renderSprite->getLayer()].push_back(*spriteIDIterator);
          renderSprite->spriteSorted();
          spriteIDIterator = m_transparentSpriteIDs[i].erase(spriteIDIterator);

          if(spriteIDIterator == m_transparentSpriteIDs[i].end())
          {
            break;
          }
        }
      }
    }

    glDepthMask(GL_FALSE);

    glEnable(GL_BLEND);
	  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for(unsigned int i = 0; i < m_transparentSpriteIDs.size(); i++)
    {
      for(std::list<ResourceHandle>::iterator spriteIDIterator = m_transparentSpriteIDs[i].begin(); spriteIDIterator != m_transparentSpriteIDs[i].end(); spriteIDIterator++)
      {
        renderSprite = m_spriteManager->getObject(*spriteIDIterator);
        if(renderSprite->getRenderable())
        {
          renderTexture = m_textureManager->getObject(renderSprite->getTextureID());

          renderTexture->setTexture(0);

		      m_spriteShader->setTexture(3, 0);
		
		      Matrix<float, 3> worldMatrix = renderSprite->getTransformationMatrix()/* * Matrix<float, 3>(1.0f / m_aspectRatio,0,0, 0,1,0, 0,0,1)*/;
		      Matrix<float, 3> textureWorldMatrix = renderSprite->getTexTransformationMatrix();
          float z = renderSprite->getLayer() / (const float)m_maxLayer;
		      m_spriteShader->setUniform(0, GL_FLOAT_MAT3, &worldMatrix[0][0]);
		      m_spriteShader->setUniform(1, GL_FLOAT_MAT3, &textureWorldMatrix[0][0]);
          m_spriteShader->setUniform(2, GL_FLOAT, &z);

          glBindBuffer(GL_ARRAY_BUFFER, m_dummyVBO);
		      glDrawArrays(GL_POINTS, 0, 1);
        }
	    }
    }

    glDisableVertexAttribArray(Shader::SPECIAL0);

    glDisable(GL_BLEND);

    glDepthMask(GL_TRUE);
  }
}

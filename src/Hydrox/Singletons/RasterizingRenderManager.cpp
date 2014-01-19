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

#include "Hydrox/Loader/RenderShaderLoader.h"
#include "Hydrox/Loader/ComputeShaderLoader.h"

namespace he
{
  RasterizerRenderManager::~RasterizerRenderManager()
  {
    glDeleteBuffers(1, &m_boneMatricesBuffer);
    glDeleteBuffers(1, &m_dummyVBO);
    glDeleteVertexArrays(1, &m_simpleMeshVAO);
    glDeleteVertexArrays(1, &m_simpleSkinnedMeshVAO);
    glDeleteVertexArrays(1, &m_simpleSkinnedTestVAO);
  }

  void RasterizerRenderManager::initialize(GLfloat aspectRatio, size_t maxSpriteLayer)
  {
    RenderManager::initialize(aspectRatio, maxSpriteLayer);

    glGenBuffers(1, &m_dummyVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_dummyVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float), nullptr, GL_STATIC_DRAW);
    glVertexAttribPointer(RenderShader::SPECIAL0, 1, GL_FLOAT, GL_FALSE, sizeof(float), NULL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glGenBuffers(1, &m_boneMatricesBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_boneMatricesBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Matrix<float, 4>) * 64, nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenVertexArrays(1, &m_simpleMeshVAO);
    glBindVertexArray(m_simpleMeshVAO);
    glVertexAttribFormat(RenderShader::POSITION, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribFormat(RenderShader::TEXTURE0, 2, GL_FLOAT, GL_FALSE, sizeof(Vector<float, 3>));
    //glVertexAttribFormat(RenderShader::NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vector<float, 3>));

    glVertexAttribBinding(RenderShader::POSITION, 0);
    glVertexAttribBinding(RenderShader::TEXTURE0, 0);
    //glVertexAttribBinding(RenderShader::NORMAL, 0);

    glEnableVertexAttribArray(RenderShader::POSITION);
    glEnableVertexAttribArray(RenderShader::TEXTURE0);
    //glEnableVertexAttribArray(RenderShader::NORMAL);
    glBindVertexArray(0);

    glGenVertexArrays(1, &m_simpleSkinnedMeshVAO);
    glBindVertexArray(m_simpleSkinnedMeshVAO);
    glVertexAttribFormat(RenderShader::POSITION, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribFormat(RenderShader::TEXTURE0, 2, GL_FLOAT, GL_FALSE, sizeof(Vector<float, 3>));
    glVertexAttribFormat(RenderShader::NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vector<float, 3>) + sizeof(Vector<float, 2>));
    glVertexAttribFormat(RenderShader::BINORMAL, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(Vector<float, 3>) + sizeof(Vector<float, 2>));
    glVertexAttribFormat(RenderShader::BONEWEIGHTS, 4, GL_FLOAT, GL_FALSE, 3 * sizeof(Vector<float, 3>) + sizeof(Vector<float, 2>));
    glVertexAttribFormat(RenderShader::BONEINDICES, 4, GL_FLOAT, GL_FALSE, sizeof(Vector<float, 4>) + 3 * sizeof(Vector<float, 3>) + sizeof(Vector<float, 2>));

    glVertexAttribBinding(RenderShader::POSITION, 0);
    glVertexAttribBinding(RenderShader::TEXTURE0, 0);
    glVertexAttribBinding(RenderShader::NORMAL, 0);
    glVertexAttribBinding(RenderShader::BINORMAL, 0);
    glVertexAttribBinding(RenderShader::BONEWEIGHTS, 0);
    glVertexAttribBinding(RenderShader::BONEINDICES, 0);

    glEnableVertexAttribArray(RenderShader::POSITION);
    glEnableVertexAttribArray(RenderShader::TEXTURE0);
    glEnableVertexAttribArray(RenderShader::NORMAL);
    glEnableVertexAttribArray(RenderShader::BINORMAL);
    glEnableVertexAttribArray(RenderShader::BONEWEIGHTS);
    glEnableVertexAttribArray(RenderShader::BONEINDICES);
    glBindVertexArray(0);

    glGenVertexArrays(1, &m_simpleSkinnedTestVAO);
    glBindVertexArray(m_simpleSkinnedTestVAO);
    glVertexAttribFormat(RenderShader::POSITION, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribFormat(RenderShader::NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vector<float, 3>));
    glVertexAttribFormat(RenderShader::BONEWEIGHTS, 4, GL_FLOAT, GL_FALSE, 2 * sizeof(Vector<float, 3>));
    glVertexAttribFormat(RenderShader::BONEINDICES, 4, GL_FLOAT, GL_FALSE, sizeof(Vector<float, 4>) + 2 * sizeof(Vector<float, 3>));

    glVertexAttribBinding(RenderShader::POSITION, 0);
    glVertexAttribBinding(RenderShader::NORMAL, 0);
    glVertexAttribBinding(RenderShader::BONEWEIGHTS, 0);
    glVertexAttribBinding(RenderShader::BONEINDICES, 0);

    glEnableVertexAttribArray(RenderShader::POSITION);
    glEnableVertexAttribArray(RenderShader::NORMAL);
    glEnableVertexAttribArray(RenderShader::BONEWEIGHTS);
    glEnableVertexAttribArray(RenderShader::BONEINDICES);
    glBindVertexArray(0);

    RenderShaderLoader renderShaderLoader;

    m_billboardHandle = renderShaderLoader.loadShader(std::string("billboard shader"), std::string("billboardShader.vert"), std::string("billboardShader.frag"),  std::string("billboardShader.geom"));

    m_spriteHandle = renderShaderLoader.loadShader(std::string("sprite shader"), std::string("spriteShader.vert"), std::string("spriteShader.frag"), std::string("spriteShader.geom"));

    m_frustumCullingGPU.initialize();

    m_cameraParameterUBO.createBuffer(sizeof(Matrix<float, 4>) * 3 + sizeof(Vector<float, 4>), GL_DYNAMIC_DRAW);
    m_cameraParameterUBO.setBindingPoint(0);
  }

  void RasterizerRenderManager::render(Matrix<float, 4>& viewMatrix, Matrix<float, 4>& projectionMatrix, Vector<float, 3>& cameraPosition, Scene *scene)
  {
	  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    Matrix<float, 4> viewProjectionMatrix = projectionMatrix * viewMatrix;
    Vector<float, 4> eyeVec = Vector<float, 4>(viewMatrix[3][0], viewMatrix[3][1], viewMatrix[3][2], 1.0f);

    m_cameraParameterUBO.setData(&viewMatrix[0][0], 0, sizeof(Matrix<float, 4>));
    m_cameraParameterUBO.setData(&projectionMatrix[0][0], sizeof(Matrix<float, 4>), sizeof(Matrix<float, 4>));
    m_cameraParameterUBO.setData(&viewProjectionMatrix[0][0], 2 * sizeof(Matrix<float, 4>), sizeof(Matrix<float, 4>));
    m_cameraParameterUBO.setData(&eyeVec[0], 3 * sizeof(Matrix<float, 4>), sizeof(Vector<float, 4>));
    m_cameraParameterUBO.uploadData();
    m_cameraParameterUBO.bindBuffer();

    ////////////////////////////////////////////CULL 3D Objects////////////////////////////////////////////

    Mesh *renderMesh;
    Material *renderMaterial;
    RenderShader *renderShader;
    Texture *renderTexture;

    const std::list<GeoNode*> renderGeometryList = scene->getMeshes();
    const std::list<AnimatedGeoNode*> renderAnimatedGeometryList = scene->getAnimatedMeshes();

    std::vector<Matrix<float, 4>> transformMatrices(renderGeometryList.size() + renderAnimatedGeometryList.size());
    std::vector<Vector<float, 4>> bbMin(renderGeometryList.size() + renderAnimatedGeometryList.size());
    std::vector<Vector<float, 4>> bbMax(renderGeometryList.size() + renderAnimatedGeometryList.size());
    std::vector<unsigned int> culledAABB;

    unsigned int k = 0;

    //collect culling data for static 3D Objects
    for(std::list<GeoNode*>::const_iterator geometryIterator = renderGeometryList.begin(); geometryIterator != renderGeometryList.end(); geometryIterator++, k++)
    {
      renderMesh = m_modelManager->getObject((*geometryIterator)->getMeshIndex());//SAVE ALLE MESHES, NO NEED TO CALL IT THEN ANYMORE, REPLACE THE OBSERVER THROUGH A ONE ELEMENT 

      transformMatrices[k] = (*geometryIterator)->getTransformationMatrix();
      memcpy(&bbMin[k][0], &renderMesh->getBBMin()[0], sizeof(Vector<float, 3>));//USE INSTANCING HERE, NEED BBOXES ONLY ONCE PER MESH
      memcpy(&bbMax[k][0], &renderMesh->getBBMax()[0], sizeof(Vector<float, 3>));
    }
    //collect culling data for skinned 3D Objects
    for(std::list<AnimatedGeoNode*>::const_iterator animatedGeometryIterator = renderAnimatedGeometryList.begin(); animatedGeometryIterator != renderAnimatedGeometryList.end(); animatedGeometryIterator++, k++)
    {
      renderMesh = m_modelManager->getObject((*animatedGeometryIterator)->getMeshIndex());

      transformMatrices[k] = (*animatedGeometryIterator)->getTransformationMatrix();
      memcpy(&bbMin[k][0], &renderMesh->getBBMin()[0], sizeof(Vector<float, 3>));//USE INSTANCING HERE, NEED BBOXES ONLY ONCE PER MESH
      memcpy(&bbMax[k][0], &renderMesh->getBBMax()[0], sizeof(Vector<float, 3>));
    }

    culledAABB.resize(renderGeometryList.size() + renderAnimatedGeometryList.size());
    m_frustumCullingGPU.cullAABB(transformMatrices, bbMin, bbMax, culledAABB);
    //culledAABB.resize(renderGeometryList.size() + renderAnimatedGeometryList.size(), 1);

	  ////////////////////////////////////////////RENDER 3D Objects////////////////////////////////////////////
    
    Matrix<float, 4> worldMatrix;
    
    glBindVertexArray(m_simpleMeshVAO);

    k = 0;
    for(std::list<GeoNode*>::const_iterator geometryIterator = renderGeometryList.begin(); geometryIterator != renderGeometryList.end(); geometryIterator++, k++)//Render 3D Objects
    {
      if((*geometryIterator)->getRenderable() && culledAABB[k])
      {
        renderMesh = m_modelManager->getObject((*geometryIterator)->getMeshIndex());
        renderMaterial = m_materialManager->getObject((*geometryIterator)->getMaterial());
        renderShader = m_renderShaderManager->getObject(renderMaterial->getShader());
        //renderTexture = m_textureManager->getObject(renderMaterial->getTexture(Material::DIFFUSETEX, 0));

        renderShader->useShader();

        worldMatrix = (*geometryIterator)->getTransformationMatrix();
        renderShader->setUniform(0, GL_FLOAT_MAT4, &(worldMatrix[0][0]));

        Vector<float, 3> color = renderMaterial->getMaterial().color;
        renderShader->setUniform(10, GL_FLOAT_VEC3, &color[0]); 

        //renderShader->setTexture(0, 0);
        //renderTexture->setTexture(0);

        renderMesh->render(0);
      }
    }

    glBindVertexArray(m_simpleSkinnedMeshVAO);

    for(std::list<AnimatedGeoNode*>::const_iterator animatedGeometryIterator = renderAnimatedGeometryList.begin(); animatedGeometryIterator != renderAnimatedGeometryList.end(); animatedGeometryIterator++, k++)//Render 3D Objects
    {
      if((*animatedGeometryIterator)->getRenderable() && culledAABB[k])
      {
        renderMesh = m_modelManager->getObject((*animatedGeometryIterator)->getMeshIndex());
        renderMaterial = m_materialManager->getObject((*animatedGeometryIterator)->getMaterial());
        renderShader = m_renderShaderManager->getObject(renderMaterial->getShader());
        //renderTexture = m_textureManager->getObject(renderMaterial->getTexture(Material::DIFFUSETEX, 0));

        renderShader->useShader();

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_boneMatricesBuffer);
        std::vector<Matrix<float, 4>> skinningMatrices = (*animatedGeometryIterator)->getSkinningMatrices();

        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(Matrix<float, 4>) * skinningMatrices.size(), &(skinningMatrices[0][0][0]));
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        Vector<float, 3> color = renderMaterial->getMaterial().color;
        renderShader->setUniform(10, GL_FLOAT_VEC3, &color[0]); 
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_boneMatricesBuffer); 
        //renderShader->setTexture(0, 0);
        //renderTexture->setTexture(0);

        renderMesh->render(0);
      }
    }
    glBindVertexArray(0);

	  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	  ////////////////////////////////////////////RENDER BILLBOARDS////////////////////////////////////////////

    const std::list<BillboardNode*> renderBillboardList = scene->getBillboards();

    Billboard *renderBillboard;

	  glEnable(GL_BLEND);
	  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnableVertexAttribArray(RenderShader::SPECIAL0);
 
    RenderShader *billboardShader = m_renderShaderManager->getObject(m_billboardHandle);
	  billboardShader->useShader();

    billboardShader->setUniform(1, GL_FLOAT_MAT4, &viewMatrix[0][0]);
    billboardShader->setUniform(2, GL_FLOAT_MAT4, &projectionMatrix[0][0]);

	  for(std::list<BillboardNode*>::const_iterator billboarditerator = renderBillboardList.begin(); billboarditerator != renderBillboardList.end(); billboarditerator++)
	  {
      renderBillboard = m_billboardManager->getObject((*billboarditerator)->getBillboardIndex());
      if(renderBillboard->getRenderable())
      {
        renderTexture = m_textureManager->getObject(renderBillboard->getTextureID());

        renderTexture->setTexture(0);
		    billboardShader->setTexture(6, 0);

        Matrix<float, 4> worldMatrix = (*billboarditerator)->getTransformationMatrix();
		    Matrix<float, 3> tmpTexTrfMatrix = renderBillboard->getTexTransformationMatrix();
        Vector<float, 2> scale = renderBillboard->getScale();
		    Vector<float, 3> translate = renderBillboard->getPosition();
        billboardShader->setUniform(0, GL_FLOAT_MAT4, &worldMatrix[0][0]);
		    billboardShader->setUniform(3, GL_FLOAT_MAT3, &tmpTexTrfMatrix[0][0]);
		    billboardShader->setUniform(4, GL_FLOAT_VEC2, &scale[0]);
		    billboardShader->setUniform(5, GL_FLOAT_VEC3, &translate[0]);

        glBindBuffer(GL_ARRAY_BUFFER, m_dummyVBO);
        glDrawArrays(GL_POINTS, 0, 1);
      }
	  }
    
    glDisable(GL_BLEND);

	  ////////////////////////////////////////////RENDER OPAQUE 2D Sprites////////////////////////////////////////////

    glClear(GL_DEPTH_BUFFER_BIT);

    Sprite *renderSprite;

    RenderShader *spriteShader = m_renderShaderManager->getObject(m_spriteHandle);
	  spriteShader->useShader();

	  for(std::list<ResourceHandle>::iterator spriteIDIterator = m_opaqueSpriteIDs.begin(); spriteIDIterator != m_opaqueSpriteIDs.end(); spriteIDIterator++)
    {
      renderSprite = m_spriteManager->getObject(*spriteIDIterator);
      if(renderSprite->getRenderable())
      {
        renderTexture = m_textureManager->getObject(renderSprite->getTextureID());

        renderTexture->setTexture(0);

		    spriteShader->setTexture(3, 0);
		
		    Matrix<float, 3> worldMatrix = renderSprite->getTransformationMatrix();
		    Matrix<float, 3> textureWorldMatrix = renderSprite->getTexTransformationMatrix();
        float z = renderSprite->getLayer() / (const float)m_maxLayer;
		    spriteShader->setUniform(0, GL_FLOAT_MAT3, &worldMatrix[0][0]);
		    spriteShader->setUniform(1, GL_FLOAT_MAT3, &textureWorldMatrix[0][0]);
        spriteShader->setUniform(2, GL_FLOAT, &z);

        glBindBuffer(GL_ARRAY_BUFFER, m_dummyVBO);
		    glDrawArrays(GL_POINTS, 0, 1);
      }
	  }

    ////////////////////////////////////////////RENDER TRANSPARENT 2D Sprites////////////////////////////////////////////

    for(unsigned int i = 0; i < m_transparentSpriteIDs.size(); i++)//resort all sprites according to their layer if their layer has been changed
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

		      spriteShader->setTexture(3, 0);
		
		      Matrix<float, 3> worldMatrix = renderSprite->getTransformationMatrix();
		      Matrix<float, 3> textureWorldMatrix = renderSprite->getTexTransformationMatrix();
          float z = renderSprite->getLayer() / (const float)m_maxLayer;
		      spriteShader->setUniform(0, GL_FLOAT_MAT3, &worldMatrix[0][0]);
		      spriteShader->setUniform(1, GL_FLOAT_MAT3, &textureWorldMatrix[0][0]);
          spriteShader->setUniform(2, GL_FLOAT, &z);

          glBindBuffer(GL_ARRAY_BUFFER, m_dummyVBO);
		      glDrawArrays(GL_POINTS, 0, 1);
        }
	    }
    }

    glDisableVertexAttribArray(RenderShader::SPECIAL0);

    glDisable(GL_BLEND);

    glDepthMask(GL_TRUE);

    m_cameraParameterUBO.unBindBuffer();
  }
}

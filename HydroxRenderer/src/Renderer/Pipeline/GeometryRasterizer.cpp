#include "Renderer/Pipeline/GeometryRasterizer.h"

#include "Renderer/RenderTree/VertexDeclarationNode.h"

#include "Renderer/RenderTree/InsertGeometryTraverser.h"
#include "Renderer/RenderTree/RemoveGeometryTraverser.h"
#include "Renderer/RenderTree/RenderGeometryTraverser.h"
#include "Renderer/RenderTree/DeleteTraverser.h"

namespace he
{
	namespace renderer
	{
    GeometryRasterizer::GeometryRasterizer(util::SingletonManager *singletonManager, unsigned int nodeCacheBlockSize)
    {
      m_modelManager = singletonManager->getService<ModelManager>();
      m_materialManager = singletonManager->getService<MaterialManager>();
      m_textureManager = singletonManager->getService<TextureManager>();
      m_renderShaderManager = singletonManager->getService<RenderShaderManager>();

      m_cameraParameterUBO.createBuffer(sizeof(util::Matrix<float, 4>) * 3 + sizeof(util::Vector<float, 4>), GL_DYNAMIC_DRAW);

      glGenVertexArrays(1, &m_simpleMeshVAO);
      glBindVertexArray(m_simpleMeshVAO);
      glVertexAttribFormat(RenderShader::POSITION, 3, GL_FLOAT, GL_FALSE, 0);
      glVertexAttribFormat(RenderShader::TEXTURE0, 2, GL_FLOAT, GL_FALSE, sizeof(util::Vector<float, 3>));
      glVertexAttribFormat(RenderShader::NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(util::Vector<float, 3>) + sizeof(util::Vector<float, 2>));
      glVertexAttribFormat(RenderShader::BINORMAL, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(util::Vector<float, 3>) + sizeof(util::Vector<float, 2>));
      glVertexAttribFormat(RenderShader::BONEWEIGHTS, 4, GL_FLOAT, GL_FALSE, 3 * sizeof(util::Vector<float, 3>) + sizeof(util::Vector<float, 2>));
      glVertexAttribFormat(RenderShader::BONEINDICES, 4, GL_FLOAT, GL_FALSE, sizeof(util::Vector<float, 4>) + 3 * sizeof(util::Vector<float, 3>) + sizeof(util::Vector<float, 2>));
      glVertexAttribFormat(RenderShader::COLOR, 4, GL_FLOAT, GL_FALSE, 2 * sizeof(util::Vector<float, 4>) + 3 * sizeof(util::Vector<float, 3>) + sizeof(util::Vector<float, 2>));

      glVertexAttribBinding(RenderShader::POSITION, 0);
      glVertexAttribBinding(RenderShader::TEXTURE0, 0);
      glVertexAttribBinding(RenderShader::NORMAL, 0);
      glVertexAttribBinding(RenderShader::BINORMAL, 0);
      glVertexAttribBinding(RenderShader::BONEWEIGHTS, 0);
      glVertexAttribBinding(RenderShader::BONEINDICES, 0);
      glVertexAttribBinding(RenderShader::COLOR, 0);
      glBindVertexArray(0);

      m_renderRootNode = new GroupNode();
    }

    GeometryRasterizer::~GeometryRasterizer()
    {
      DeleteTraverser deleteTraverser;
      deleteTraverser.doTraverse(m_renderRootNode);

      delete m_renderRootNode;

      glDeleteVertexArrays(1, &m_simpleMeshVAO);
    }

    void GeometryRasterizer::insertGeometry(sg::GeoNode *node)
    {
      InsertGeometryTraverser insertTraverser(m_modelManager, m_materialManager, m_renderShaderManager);
      insertTraverser.setNode(node);
      insertTraverser.doTraverse(m_renderRootNode);
    }

    void GeometryRasterizer::removeGeometry(sg::GeoNode *node)
    {
      RemoveGeometryTraverser removeTraverser(m_modelManager, m_materialManager);
      removeTraverser.setNode(node);
      removeTraverser.doTraverse(m_renderRootNode);
    }

    void GeometryRasterizer::rasterizeGeometry(util::Matrix<float, 4>& viewMatrix, util::Matrix<float, 4>& projectionMatrix, util::Vector<float, 3>& cameraPosition)
    {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

      util::Matrix<float, 4> viewProjectionMatrix = projectionMatrix * viewMatrix;
      util::Vector<float, 4> eyeVec = util::Vector<float, 4>(viewMatrix[3][0], viewMatrix[3][1], viewMatrix[3][2], 1.0f);

      m_cameraParameterUBO.setData(&viewMatrix[0][0], 0, sizeof(util::Matrix<float, 4>));
      m_cameraParameterUBO.setData(&projectionMatrix[0][0], sizeof(util::Matrix<float, 4>), sizeof(util::Matrix<float, 4>));
      m_cameraParameterUBO.setData(&viewProjectionMatrix[0][0], 2 * sizeof(util::Matrix<float, 4>), sizeof(util::Matrix<float, 4>));
      m_cameraParameterUBO.setData(&eyeVec[0], 3 * sizeof(util::Matrix<float, 4>), sizeof(util::Vector<float, 4>));
      m_cameraParameterUBO.uploadData();
      m_cameraParameterUBO.bindBuffer(0);

      glBindVertexArray(m_simpleMeshVAO);

      RenderGeometryTraverser renderTraverser(m_modelManager, m_materialManager, m_renderShaderManager, m_textureManager);
      renderTraverser.doTraverse(m_renderRootNode);

      glBindVertexArray(0);
    }
  }
}
#include "Renderer/Pipeline/SkyboxRenderer.h"

#include <vector>

#include <Utilities/PrimitiveGenerator/CubeGenerator.h>

namespace he
{
  namespace renderer
  {
    SkyboxRenderer::SkyboxRenderer()
    {
    }

    SkyboxRenderer::~SkyboxRenderer()
    {
      glDeleteVertexArrays(1, &m_skyBoxVAO);
    }

    void SkyboxRenderer::initialize(util::SingletonManager *singletonManager, util::ResourceHandle skyboxTextureHandles[6])
    {
      m_shaderContainer = singletonManager->getService<sh::ShaderContainer>();
      m_textureManager = singletonManager->getService<db::TextureManager>();
      m_textureArrayManager = singletonManager->getService<db::TextureArrayManager>();

      m_skyboxShaderHandle = m_shaderContainer->getRenderShaderHandle(sh::ShaderContainer::SKYBOX, sh::ShaderSlotFlags(3));

      db::Texture2D *texture = m_textureManager->getObject(skyboxTextureHandles[0]);
      
      unsigned int size = texture->getResolution()[0] * texture->getResolution()[1] * (texture->getBitsPerComponent() / 8) * 4;

      std::vector<GLubyte> data(6 * size);

      for (unsigned int i = 0; i < 6; i++)
      {
        texture = m_textureManager->getObject(skyboxTextureHandles[i]);
        texture->getTextureData(&data[i * size]);
      }

      m_arrayTextureHandle = m_textureArrayManager->addObject(db::Texture3D(texture->getResolution()[0], texture->getResolution()[1], 6, GL_TEXTURE_2D_ARRAY, texture->getType(), texture->getInternalFormat(), texture->getFormat(), texture->getChannelNumber(), texture->getBitsPerComponent(), &data[0]));

      std::vector<util::vec3f> positions;

      util::CubeGenerator::generateCube(positions, UINT_MAX);

      std::vector<float> geometryData(positions.size() * 5);
      for (unsigned int i = 0; i < 12; i++)
      {
        positions[i * 3 + 0] *= 0.25f;
        positions[i * 3 + 1] *= 0.25f;
        positions[i * 3 + 2] *= 0.25f;

        unsigned int index = i * 3 * 5;
        //first
        geometryData[index + 0] = positions[i * 3 + 0][0];
        geometryData[index + 1] = positions[i * 3 + 0][1];
        geometryData[index + 2] = positions[i * 3 + 0][2];

        geometryData[index + 3] = 0;// i % 2;
        geometryData[index + 4] = 1;// (i + 1) % 2;

        //second
        geometryData[index + 10] = positions[i * 3 + 1][0];
        geometryData[index + 11] = positions[i * 3 + 1][1];
        geometryData[index + 12] = positions[i * 3 + 1][2];

        geometryData[index + 13] = (i + 1) % 2;
        geometryData[index + 14] = 0;// i % 2;

        //third
        geometryData[index + 5] = positions[i * 3 + 2][0];
        geometryData[index + 6] = positions[i * 3 + 2][1];
        geometryData[index + 7] = positions[i * 3 + 2][2];

        geometryData[index + 8] = 1;// (i + 1) % 2;
        geometryData[index + 9] = (i + 1) % 2;
      }

      m_geometryData.createBuffer(GL_ARRAY_BUFFER, sizeof(float) * 5 * positions.size(), sizeof(float) * 5 * positions.size(), GL_STATIC_DRAW, &geometryData[0]);

      glGenVertexArrays(1, &m_skyBoxVAO);
      glBindVertexArray(m_skyBoxVAO);

      glVertexAttribFormat(sh::RenderShader::POSITION, 3, GL_FLOAT, GL_FALSE, 0);
      glVertexAttribFormat(sh::RenderShader::TEXTURE0, 2, GL_FLOAT, GL_FALSE, sizeof(util::vec3f));

      glVertexAttribBinding(sh::RenderShader::POSITION, 0);
      glVertexAttribBinding(sh::RenderShader::TEXTURE0, 0);

      glEnableVertexAttribArray(sh::RenderShader::POSITION);
      glEnableVertexAttribArray(sh::RenderShader::TEXTURE0);

      glBindVertexArray(0);
    }

    void SkyboxRenderer::render() const
    {
      glDepthFunc(GL_LEQUAL);

      glBindVertexArray(m_skyBoxVAO);

      db::Texture3D *renderTexture = m_textureArrayManager->getObject(m_arrayTextureHandle);
      const sh::RenderShader& skyboxShader = m_shaderContainer->getRenderShader(m_skyboxShaderHandle);

      skyboxShader.useShader();
      renderTexture->setTexture(0, 0);

      m_geometryData.bindVertexbuffer(0, 0, sizeof(util::vec3f) + sizeof(util::vec2f));

      glDrawArrays(GL_TRIANGLES, 0, 36);

      m_geometryData.unbindVertexBuffer(0);

      renderTexture->unsetTexture(0);
      skyboxShader.useNoShader();
      
      glBindVertexArray(0);

      glDepthFunc(GL_LESS);
    }
  }
}

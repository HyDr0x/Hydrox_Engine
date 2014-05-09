#include "Renderer/Pipeline/SkyboxRenderer.h"

#include <vector>
#include <Utilities\PrimitiveGenerator\CubeGenerator.h>

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

    void SkyboxRenderer::initialize(util::SingletonManager *singletonManager, util::ResourceHandle skyboxShaderHandle, util::ResourceHandle skyboxTextureHandles[6])
    {
      m_renderShaderManager = singletonManager->getService<RenderShaderManager>();
      m_textureManager = singletonManager->getService<TextureManager>();
      m_textureArrayManager = singletonManager->getService<TextureArrayManager>();

      m_skyboxShaderHandle = skyboxShaderHandle;

      Texture2D *texture;
      texture = m_textureManager->getObject(skyboxTextureHandles[0]);
      unsigned int size = texture->getTextureSize();

      std::vector<unsigned char> data(size * 6);

      for (unsigned int i = 0; i < 6; i++)
      {
        texture = m_textureManager->getObject(skyboxTextureHandles[i]);
        texture->getTextureData(&data[i * size]);
      }

      m_arrayTextureHandle = m_textureArrayManager->addObject(Texture3D(texture->getResolution()[0], texture->getResolution()[1], 6, GL_TEXTURE_2D_ARRAY, texture->getType(), texture->getInternalFormat(), texture->getFormat(), &data[0]));

      std::vector<util::Vector<float, 3>> positions;

      util::CubeGenerator::generateCube(positions);

      std::vector<float> geometryData(positions.size() * 5);
      for (unsigned int i = 0; i < 12; i++)
      {
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

      m_geometryData.createBuffer(GL_ARRAY_BUFFER, sizeof(float) * 5 * positions.size(), 0, GL_STATIC_DRAW, &geometryData[0]);

      glGenVertexArrays(1, &m_skyBoxVAO);
      glBindVertexArray(m_skyBoxVAO);

      glVertexAttribFormat(RenderShader::POSITION, 3, GL_FLOAT, GL_FALSE, 0);
      glVertexAttribFormat(RenderShader::TEXTURE0, 2, GL_FLOAT, GL_FALSE, sizeof(util::Vector<float, 3>));

      glVertexAttribBinding(RenderShader::POSITION, 0);
      glVertexAttribBinding(RenderShader::TEXTURE0, 0);

      glEnableVertexAttribArray(RenderShader::POSITION);
      glEnableVertexAttribArray(RenderShader::TEXTURE0);

      glBindVertexArray(0);
    }

    void SkyboxRenderer::render() const
    {
      glDepthFunc(GL_LEQUAL);

      glBindVertexArray(m_skyBoxVAO);

      Texture3D *renderTexture = m_textureArrayManager->getObject(m_arrayTextureHandle);
      RenderShader *skyboxShader = m_renderShaderManager->getObject(m_skyboxShaderHandle);

      skyboxShader->useShader();
      renderTexture->setTexture(0, 0);

      m_geometryData.bindVertexbuffer(0, 0, sizeof(util::Vector<float, 3>) + sizeof(util::Vector<float, 2>));

      glDrawArrays(GL_TRIANGLES, 0, 36);

      m_geometryData.unbindVertexBuffer(0);

      renderTexture->unsetTexture();
      skyboxShader->useNoShader();
      
      glBindVertexArray(0);

      glDepthFunc(GL_LESS);
    }
	}
}

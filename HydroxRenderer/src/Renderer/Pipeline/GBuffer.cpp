#include "Renderer/Pipeline/GBuffer.h"

#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <DataBase/Texture2D.h>

#include "Renderer/Pipeline/RenderOptions.h"

namespace he
{
  namespace renderer
  {
    GBuffer::GBuffer()
    {
    }

    GBuffer::~GBuffer()
    {
    }

    void GBuffer::initialize(util::SingletonManager *singletonManager)
    {
      m_singletonManager = singletonManager;

      util::SharedPointer<RenderOptions> options = m_singletonManager->getService<RenderOptions>();

      m_linearDepthBufferCreation = sh::ShaderContainer::LINEARDEPTHBUFFERCREATION;

      m_depthTexture = util::SharedPointer<db::Texture2D>(new db::Texture2D(options->width, options->height, GL_TEXTURE_2D, GL_FLOAT, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, 1, 32));
      m_linearDepthTexture = util::SharedPointer<db::Texture2D>(new db::Texture2D(options->width, options->height, GL_TEXTURE_2D, GL_FLOAT, GL_R32F, GL_RED, 1, 32));
      m_colorTexture = util::SharedPointer<db::Texture2D>(new db::Texture2D(options->width, options->height, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA16F, GL_RGBA, 4, 16));
      m_normalTexture = util::SharedPointer<db::Texture2D>(new db::Texture2D(options->width, options->height, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA16F, GL_RGBA, 4, 16));
      m_vertexNormalTexture = util::SharedPointer<db::Texture2D>(new db::Texture2D(options->width, options->height, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA16F, GL_RGBA, 4, 16));
      m_materialTexture = util::SharedPointer<db::Texture2D>(new db::Texture2D(options->width, options->height, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA16F, GL_RGBA, 4, 16));
      
      m_fullscreenRenderQuad.setRenderTargets(m_depthTexture, 4, m_colorTexture, m_normalTexture, m_materialTexture, m_vertexNormalTexture);
      m_fullscreenColorRenderquad.setRenderTargets(m_depthTexture, 1, m_colorTexture);

      createTestGBuffer();
    }

    void GBuffer::setGBuffer() const
    {
      m_fullscreenRenderQuad.setWriteFrameBuffer();
    }

    void GBuffer::unsetGBuffer() const
    {
      m_fullscreenRenderQuad.unsetWriteFrameBuffer();
    }

    void GBuffer::setColorBuffer() const
    {
      m_fullscreenColorRenderquad.setWriteFrameBuffer();
    }

    void GBuffer::unsetColorBuffer() const
    {
      m_fullscreenColorRenderquad.unsetWriteFrameBuffer();
    }

    void GBuffer::calculateLinearDepthBuffer() const
    {
      float clearValue = FLT_MAX;
      m_linearDepthTexture->clearTexture(&clearValue);

      sh::ComputeShader linearDepthBufferCreationShader = m_singletonManager->getService<sh::ShaderContainer>()->getComputeShader(m_linearDepthBufferCreation);

      linearDepthBufferCreationShader.useShader();
      m_linearDepthTexture->bindImageTexture(0, 0, GL_WRITE_ONLY, GL_R32F);
      m_depthTexture->setTexture(0, 0);

      sh::ComputeShader::dispatchComputeShader(128, 1, 1);

      m_depthTexture->unsetTexture(0);
      m_linearDepthTexture->unbindImageTexture(0, 0, GL_WRITE_ONLY, GL_R32F);
      linearDepthBufferCreationShader.useNoShader();
    }

    void GBuffer::setClearColor(util::vec4f clearColor)
    {
      m_clearColor = clearColor;
    }

    void GBuffer::useTestGBuffer()
    {
      m_colorTexture->setTextureData(0, 0, m_colorTexture->getResolution()[0], m_colorTexture->getResolution()[1], &m_colorBuffer[0][0]);
      m_normalTexture->setTextureData(0, 0, m_normalTexture->getResolution()[0], m_normalTexture->getResolution()[1], &m_normalBuffer[0][0]);
      m_vertexNormalTexture->setTextureData(0, 0, m_vertexNormalTexture->getResolution()[0], m_vertexNormalTexture->getResolution()[1], &m_vertexNormaluffer[0][0]);
      m_materialTexture->setTextureData(0, 0, m_materialTexture->getResolution()[0], m_materialTexture->getResolution()[1], &m_materialBuffer[0][0]);
      m_linearDepthTexture->setTextureData(0, 0, m_linearDepthTexture->getResolution()[0], m_linearDepthTexture->getResolution()[1], &m_linearDepthBuffer[0]);
      m_depthTexture->setTextureData(0, 0, m_depthTexture->getResolution()[0], m_depthTexture->getResolution()[1], &m_depthBuffer[0]);
    }

    void GBuffer::createTestGBuffer()
    {
      util::SharedPointer<RenderOptions> options = m_singletonManager->getService<RenderOptions>();
      m_colorBuffer.resize(options->width * options->height);
      m_normalBuffer.resize(options->width * options->height);
      m_vertexNormaluffer.resize(options->width * options->height);
      m_materialBuffer.resize(options->width * options->height);
      m_depthBuffer.resize(options->width * options->height);
      m_linearDepthBuffer.resize(options->width * options->height);

      const unsigned int tileNumberX = (options->width + options->cacheTileSize - 1) / options->cacheTileSize;
      const unsigned int tileNumberY = (options->height + options->cacheTileSize - 1) / options->cacheTileSize;

      const float normalAnglePhiQ = (2.0f * util::math::PI) / options->cacheTileSize;
      const float normalAngleThetaQ = util::math::PI / options->cacheTileSize;

      const float depthQ = (options->farPlane - options->nearPlane) / (options->cacheTileSize * options->cacheTileSize);

      unsigned int tmpCounter = 0;
      for(unsigned int ty = 0; ty < tileNumberY; ty++)
      {
        for(unsigned int tx = 0; tx < tileNumberX; tx++)
        {
          unsigned int tileIndexOffset = (tx * tileNumberY + ty);

          for(unsigned int y = 0; y < options->cacheTileSize; y++)
          {
            for(unsigned int x = 0; x < options->cacheTileSize; x++)
            {
              unsigned int coordX = tx * options->cacheTileSize + x;
              unsigned int coordY = ty * options->cacheTileSize + y;
              unsigned int index = coordX + coordY * options->width;

              if(index < m_colorBuffer.size())
              {
                //unsigned int index = tmpCounter;
                tmpCounter++;

                m_colorBuffer[index] = util::vec4f(tx / float(tileNumberX), ty / float(tileNumberY), (tx / float(tileNumberX) + ty / float(tileNumberY)) / 2.0f, 1.0f);

                float nx = cos(x * normalAnglePhiQ);
                float ny = sin(x * normalAnglePhiQ);
                float nz = cos(y * normalAngleThetaQ);
                m_normalBuffer[index] = util::vec4f(nx, ny, nz, 0.0f).normalize();
                m_vertexNormaluffer[index] = m_normalBuffer[index];

                m_materialBuffer[index] = util::vec4f(x % 2, y % 2, (x + y * options->cacheTileSize) / float(options->cacheTileSize * options->cacheTileSize), 0.0f);

                m_linearDepthBuffer[index] = float(x + y * options->cacheTileSize) * depthQ;

                m_depthBuffer[index] = ((-m_linearDepthBuffer[index] * (-(options->farPlane + options->nearPlane) / (options->farPlane - options->nearPlane))) + ((-2.0f * options->farPlane * options->nearPlane) / (options->farPlane - options->nearPlane))) / m_linearDepthBuffer[index];
              }
            }
          }
        }
      }
    }

    util::SharedPointer<db::Texture2D> GBuffer::getDepthTexture() const
    {
      return m_depthTexture;
    }

    util::SharedPointer<db::Texture2D> GBuffer::getLinearDepthTexture() const
    {
      return m_linearDepthTexture;
    }

    util::SharedPointer<db::Texture2D> GBuffer::getColorTexture() const
    {
      return m_colorTexture;
    }

    util::SharedPointer<db::Texture2D> GBuffer::getNormalTexture() const
    {
      return m_normalTexture;
    }

    util::SharedPointer<db::Texture2D> GBuffer::getVertexNormalTexture() const
    {
      return m_vertexNormalTexture;
    }

    util::SharedPointer<db::Texture2D> GBuffer::getMaterialTexture() const
    {
      return m_materialTexture;
    }

    void GBuffer::clear() const
    {
      m_fullscreenRenderQuad.clearTargets(1.0f, std::vector<util::vec4f>(3, m_clearColor), true);
    }
  }
}
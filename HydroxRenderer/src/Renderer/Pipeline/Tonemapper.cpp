#include "Renderer/Pipeline/Tonemapper.h"

#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include "Renderer/Pipeline/RenderOptions.h"

namespace he
{
  namespace renderer
  {
    Tonemapper::Tonemapper() : m_histogramBins(16), m_pingPong(0)
    {
    }

    Tonemapper::~Tonemapper()
    {
      glDeleteVertexArrays(1, &m_histogramVAO);
      glDeleteBuffers(2, m_pboIndex);
    }

    void Tonemapper::initialize(util::SingletonManager *singletonManager)
    {
      m_logLuminanceRange = util::vec2f(0, 0);
      m_options = singletonManager->getService<RenderOptions>();
      m_renderShaderContainer = singletonManager->getService<sh::ShaderContainer>();
      util::SharedPointer<sh::ShaderContainer> renderShader = singletonManager->getService<sh::ShaderContainer>();

      m_histogramShaderHandle = m_renderShaderContainer->getRenderShaderHandle(sh::ShaderContainer::HISTOGRAM, sh::ShaderSlotFlags(1));
      m_tonemappingShaderHandle = m_renderShaderContainer->getRenderShaderHandle(sh::ShaderContainer::TONEMAPPING, sh::ShaderSlotFlags(8192));

      createHistogramVertices();

      m_timer.start();//to get a better starting time
    }

    void Tonemapper::doToneMapping(util::SharedPointer<db::Texture2D> combinedTexture)
    {
      createHistogram(combinedTexture);
      toneMapping(combinedTexture);
    }

    void Tonemapper::createHistogramVertices()
    {
      std::vector<util::vec2f> vertices(m_options->width * m_options->height);

      for(unsigned int i = 0; i < m_options->width * m_options->height; i++)
      {
        vertices[i] = util::vec2f(float(i % m_options->width) / float(m_options->width), float(i / m_options->width) / float(m_options->height));
      }

      m_histogramVertices.createBuffer(GL_ARRAY_BUFFER, m_options->width * m_options->height * sizeof(util::vec2f), m_options->width * m_options->height * sizeof(util::vec2f), GL_STATIC_DRAW, &vertices[0]);

      m_histogram[0] = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_histogramBins, 1, GL_TEXTURE_2D, GL_FLOAT, GL_R32F, GL_RED, 1, 32));
      m_histogram[1] = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_histogramBins, 1, GL_TEXTURE_2D, GL_FLOAT, GL_R32F, GL_RED, 1, 32));

      glGenBuffers(2, m_pboIndex);

      glBindBuffer(GL_PIXEL_PACK_BUFFER, m_pboIndex[0]);
      glBufferData(GL_PIXEL_PACK_BUFFER, m_histogramBins * sizeof(GLfloat), nullptr, GL_STREAM_READ);
      glBindBuffer(GL_PIXEL_PACK_BUFFER, m_pboIndex[1]);
      glBufferData(GL_PIXEL_PACK_BUFFER, m_histogramBins * sizeof(GLfloat), nullptr, GL_STREAM_READ);
      glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

      glGenVertexArrays(1, &m_histogramVAO);

      glBindVertexArray(m_histogramVAO);
      glVertexAttribFormat(sh::RenderShader::POSITION, 2, GL_FLOAT, GL_FALSE, 0);
      glVertexAttribBinding(sh::RenderShader::POSITION, 0);
      glBindVertexArray(0);
    }

    void Tonemapper::createHistogram(util::SharedPointer<db::Texture2D> combinedTexture)
    {
      m_histogramRenderQuad.setRenderTargets(1, m_histogram[m_pingPong]);
      m_histogramRenderQuad.clearTargets(0, util::vec4f(0, 0, 0, 0));

      glDepthMask(GL_FALSE);
      glEnable(GL_BLEND);
      glBlendFunc(GL_ONE, GL_ONE);
      glViewport(0, 0, m_histogramBins, 1);

      m_histogramRenderQuad.setWriteFrameBuffer();
      const sh::RenderShader& shader = m_renderShaderContainer->getRenderShader(m_histogramShaderHandle);
      shader.useShader();

      glBindVertexArray(m_histogramVAO);
      glEnableVertexAttribArray(sh::RenderShader::POSITION);
      m_histogramVertices.bindVertexbuffer(0, 0, sizeof(util::vec2f));

      combinedTexture->setTexture(0, 0);

      glDrawArrays(GL_POINTS, 0, m_options->width * m_options->height);

      combinedTexture->unsetTexture(0);

      m_histogramVertices.unbindVertexBuffer(0);
      glDisableVertexAttribArray(sh::RenderShader::POSITION);
      glBindVertexArray(0);

      shader.useNoShader();
      m_histogramRenderQuad.unsetWriteFrameBuffer();

      glViewport(0, 0, m_options->width, m_options->height);
      glDisable(GL_BLEND);
      glDepthMask(GL_TRUE);

      m_histogramRenderQuad.setReadFrameBuffer(GL_COLOR_ATTACHMENT0);
      glBindBuffer(GL_PIXEL_PACK_BUFFER, m_pboIndex[m_pingPong]);

      glReadPixels(0, 0, m_histogramBins, 1, GL_RED, GL_FLOAT, nullptr);

      glBindBuffer(GL_PIXEL_PACK_BUFFER, m_pboIndex[(m_pingPong + 1) % 2]);
      m_histogramRenderQuad.unsetReadFrameBuffer();

      std::vector<GLfloat> histogram(m_histogramBins);
      GLfloat *data = (GLfloat*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
      std::copy(data, data + m_histogramBins, &histogram[0]);
      glUnmapBuffer(GL_PIXEL_PACK_BUFFER);

      glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

      m_pingPong = 1 - m_pingPong;

      unsigned int pixelNumber = 0, maxPixelNumber = 0;

      for(unsigned int i = 0; i < histogram.size(); i++)
      {
        pixelNumber += histogram[i];
        if(i >= m_options->usedHistogramBins)
        {
          pixelNumber -= histogram[i - m_options->usedHistogramBins];
        }

        if(pixelNumber > maxPixelNumber)
        {
          maxPixelNumber = pixelNumber;
          m_logLuminanceRangeNew[1] = i;
          for(m_logLuminanceRangeNew[0] = int(i) - int(m_options->usedHistogramBins); histogram[std::max<int>(m_logLuminanceRangeNew[0], 0)] == 0 && m_logLuminanceRangeNew[0] < m_logLuminanceRangeNew[1]; m_logLuminanceRangeNew[0]++)
          {}
        }
      }
    }

    void Tonemapper::toneMapping(util::SharedPointer<db::Texture2D> combinedTexture)
    {
      m_logLuminanceRangeNew[0] = std::max<int>(m_logLuminanceRangeNew[0], -1);

      util::vec2f luminanceDifference = util::math::vector_cast<float>(m_logLuminanceRangeNew) - m_logLuminanceRange;

      luminanceDifference[0] = luminanceDifference[0] > 0.0f ? 1.0f : luminanceDifference[0] < 0.0f ? -1.0f : 0.0f;
      luminanceDifference[1] = luminanceDifference[1] > 0.0f ? 1.0f : luminanceDifference[1] < 0.0f ? -1.0f : 0.0f;

      m_timer.stop();

      if(luminanceDifference[0] != 0 || luminanceDifference[1] != 0)
      {
        float timeDiff = static_cast<util::time>(float(m_timer.getTime()) * 0.001f);

        m_logLuminanceRange += luminanceDifference * timeDiff * m_options->logLuminancePerMS;

        m_logLuminanceRange[0] = std::min<float>(m_logLuminanceRange[0], m_histogramBins);
        m_logLuminanceRange[1] = std::min<float>(m_logLuminanceRange[1], m_histogramBins);

        m_logLuminanceRange[0] = std::max<float>(m_logLuminanceRange[0], -1.0f);
        m_logLuminanceRange[1] = std::max<float>(m_logLuminanceRange[1], -1.0f);

        if(luminanceDifference[0] > 0) m_logLuminanceRange[0] = util::math::clamp<float>(m_logLuminanceRange[0], m_logLuminanceRange[0], m_logLuminanceRangeNew[0]);
        else m_logLuminanceRange[0] = util::math::clamp<float>(m_logLuminanceRange[0], m_logLuminanceRangeNew[0], m_logLuminanceRange[0]);

        if(luminanceDifference[1] > 0) m_logLuminanceRange[1] = util::math::clamp<float>(m_logLuminanceRange[1], m_logLuminanceRange[1], m_logLuminanceRangeNew[1]);
        else m_logLuminanceRange[1] = util::math::clamp<float>(m_logLuminanceRange[1], m_logLuminanceRangeNew[1], m_logLuminanceRange[1]);

        m_luminanceRange[0] = m_logLuminanceRange[0] < 0 ? util::math::clamp(m_luminanceRange[0] + luminanceDifference[0] * timeDiff * m_options->logLuminancePerMS, 0.0f, 1.0f) : std::pow(2.0f, m_logLuminanceRange[0]);
        m_luminanceRange[1] = std::pow(2.0f, m_logLuminanceRange[1]);

      }

      m_timer.start();

      const sh::RenderShader& toneMappingShader = m_renderShaderContainer->getRenderShader(m_tonemappingShaderHandle);
      toneMappingShader.useShader();

      //float s = 0.00390625f * m_luminanceRange[1] / (1.0f * m_luminanceRange[0] + 0.0001f);
      sh::RenderShader::setUniform(2, GL_FLOAT, &m_options->s);
      sh::RenderShader::setUniform(3, GL_FLOAT_VEC2, &m_luminanceRange[0]);

      combinedTexture->setTexture(0, 0);

      m_histogramRenderQuad.render();

      combinedTexture->unsetTexture(0);

      toneMappingShader.useNoShader();
    }
  }
}

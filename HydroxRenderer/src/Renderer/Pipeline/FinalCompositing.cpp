#include "Renderer/Pipeline/FinalCompositing.h"

#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <DataBase/RenderShader.h>

#include "Renderer/Pipeline/RenderShaderContainer.h"
#include "Renderer/Pipeline/RenderOptions.h"

namespace he
{
  namespace renderer
  {
    FinalCompositing::FinalCompositing()
    {
    }

    FinalCompositing::~FinalCompositing()
    {
    }

    void FinalCompositing::initialize(util::SingletonManager *singletonManager)
    {
      m_options = singletonManager->getService<RenderOptions>();
      m_renderShaderManager = singletonManager->getService<db::RenderShaderManager>();
      util::SharedPointer<RenderShaderContainer> renderShader = singletonManager->getService<RenderShaderContainer>();

      m_offscreenBufferShaderHandle = renderShader->offscreenBufferShaderHandle;
      m_combineShaderHandle = renderShader->combineShaderHandle;

      m_combinedImage = util::SharedPointer<db::Texture2D>(new db::Texture2D(m_options->width, m_options->height, GL_TEXTURE_2D, GL_FLOAT, GL_RGBA16F, GL_RGBA, 4, 64));

      m_fullscreenRenderQuad.setRenderTargets(1, m_combinedImage);
    }

    void FinalCompositing::composeImage(util::SharedPointer<db::Texture2D> colorTexture,
      util::SharedPointer<db::Texture2D> directlightTexture,
      util::SharedPointer<db::Texture2D> indirectlightTexture) const
    {
      db::RenderShader *shader = m_renderShaderManager->getObject(m_combineShaderHandle);

      shader->useShader();
      m_fullscreenRenderQuad.setWriteFrameBuffer();
      colorTexture->setTexture(0, 0);
      directlightTexture->setTexture(1, 1);
      indirectlightTexture->setTexture(2, 2);

      m_fullscreenRenderQuad.render();

      indirectlightTexture->unsetTexture();
      directlightTexture->unsetTexture();
      colorTexture->unsetTexture();
      m_fullscreenRenderQuad.unsetWriteFrameBuffer();
      shader->useNoShader();
    }

    void FinalCompositing::renderDebugOutput(util::SharedPointer<db::Texture2D> texture) const
    {
      db::RenderShader *shader = m_renderShaderManager->getObject(m_offscreenBufferShaderHandle);

      shader->useShader();
      texture->setTexture(0, 0);

      m_fullscreenRenderQuad.render();

      texture->unsetTexture();
      shader->useNoShader();
    }

    util::SharedPointer<db::Texture2D> FinalCompositing::getCombinedTexture() const
    {
      return m_combinedImage;
    }

    /*
    void FinalCompositing::downSampling()
    {
      util::vec2f filterImageSize = util::vec2f(4, 1.0f / (m_options->width * m_options->height));
      float filterSampling = 1.0f / float(filterImageSize[0]);
      
      float newWidth = m_options->width;
      float newHeight = m_options->height;
      unsigned int downSamplingSteps = std::max(ceil(log(newWidth) / log(float(filterImageSize[0]))), ceil(log(newHeight) / log(float(filterImageSize[0]))));
      util::vec2i samplingV(1.0f, 0.0f), samplingH(0.0f, 1.0f);
      util::vec2i normalizeTexCoord(newWidth, newHeight);
      util::vec2f textureOffset(1.0f / (2.0f * newWidth), 1.0f / (2.0f * newHeight));

      db::RenderShader *shader = m_renderShaderManager->getObject(m_histogramShaderHandle);
      shader->useShader();
      db::RenderShader::setUniform(2, GL_FLOAT_VEC2, &filterImageSize[0]);

      for(unsigned int i = 0; i < downSamplingSteps; i++)
      {
        if(filterImageSize[0] > newWidth)
        {
          filterImageSize[0] = newWidth;
          db::RenderShader::setUniform(2, GL_FLOAT_VEC2, &filterImageSize[0]);
        }
        db::RenderShader::setUniform(1, GL_INT_VEC2, &samplingV[0]);
        db::RenderShader::setUniform(3, GL_INT_VEC2, &normalizeTexCoord[0]);
        
        
        newWidth = std::max(newWidth * filterSampling, 1.0f);
        glViewport(0, 0, newWidth, newHeight);
        normalizeTexCoord[0] = newWidth;
        textureOffset[0] = 1.0f / (2.0f * newWidth);
        db::RenderShader::setUniform(4, GL_FLOAT_VEC2, &textureOffset[0]);

        m_downSampleImage0->setTexture(0, 0);
        m_downSamplingRenderQuad.setRenderTargets(1, m_downSampleImage1);
        m_downSamplingRenderQuad.setWriteFrameBuffer();
        m_downSamplingRenderQuad.render();
        m_downSamplingRenderQuad.unsetWriteFrameBuffer();
        m_downSampleImage0->unsetTexture();

        std::swap(m_downSampleImage0, m_downSampleImage1);

        std::vector<util::vec4f> pixels0(m_options->width * m_options->height);
        m_downSampleImage0->getTextureData(&pixels0[0]);

        for(unsigned int j = 0; j < pixels0.size(); j++)
        {
          if(pixels0[j][0] > 9.0f)
          {
            int g = pixels0[j][0];
            int h = 0;
          }
        }

        if(i == 0)
        {
          filterImageSize[1] = 0;
          db::RenderShader::setUniform(2, GL_FLOAT_VEC2, &filterImageSize[0]);
        }

        if(filterImageSize[0] > newHeight)
        {
          filterImageSize[0] = newHeight;
          db::RenderShader::setUniform(2, GL_FLOAT_VEC2, &filterImageSize[0]);
        }
        db::RenderShader::setUniform(1, GL_INT_VEC2, &samplingH[0]);
        db::RenderShader::setUniform(3, GL_INT_VEC2, &normalizeTexCoord[0]);

        newHeight = std::max(newHeight * filterSampling, 1.0f);
        glViewport(0, 0, newWidth, newHeight);
        normalizeTexCoord[1] = newHeight;
        textureOffset[1] = 1.0f / (2.0f * newHeight);
        db::RenderShader::setUniform(4, GL_FLOAT_VEC2, &textureOffset[0]);

        m_downSampleImage0->setTexture(0, 0);
        m_downSamplingRenderQuad.setRenderTargets(1, m_downSampleImage1);
        m_downSamplingRenderQuad.setWriteFrameBuffer();
        m_downSamplingRenderQuad.render();
        m_downSamplingRenderQuad.unsetWriteFrameBuffer();
        m_downSampleImage0->unsetTexture();

        std::swap(m_downSampleImage0, m_downSampleImage1);

        std::vector<util::vec4f> pixels1(m_options->width * m_options->height);
        m_downSampleImage0->getTextureData(&pixels1[0]);

        for(unsigned int j = 0; j < pixels1.size(); j++)
        {
          if(pixels1[j][0] > 9.0f)
          { 
            int g = pixels1[j][0];
            int h = 0;
          }
        }
      }
      shader->useNoShader();

      std::vector<util::vec4f> pixels(m_options->width * m_options->height);
      m_downSampleImage0->getTextureData(&pixels[0]);

      glViewport(0, 0, m_options->width, m_options->height);
      
      db::RenderShader *toneMappingShader = m_renderShaderManager->getObject(m_tonemappingShaderHandle);
      toneMappingShader->useShader();

      float s = 2.5f;
      float halfLuminanceRange = 25.0f;
      db::RenderShader::setUniform(2, GL_FLOAT, &s);
      db::RenderShader::setUniform(3, GL_FLOAT, &halfLuminanceRange);

      m_tonemappingImage->setTexture(0, 0);
      m_downSampleImage0->setTexture(1, 1);//maxLuminance at (0, 0)

      m_toneMappingRenderQuad.render();

      m_tonemappingImage->unsetTexture();
      m_downSampleImage1->unsetTexture();

      toneMappingShader->useNoShader();
    }*/
  }
}

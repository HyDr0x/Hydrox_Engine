#ifndef TONEMAPPER_H_
#define TONEMAPPER_H_

#include <list>

#include <GL/glew.h>

#include <Utilities/Timer/Timer.h>

#include <DataBase/Texture2D.h>
#include <DataBase/ResourceManager.hpp>

#include "Renderer/Buffer/GPUBuffer.h"
#include "Renderer/Pipeline/Renderquad.h"

namespace he
{
  namespace util
  {
    class SingletonManager;
  }

  namespace renderer
  {
    class RenderOptions;

    class Tonemapper
    {
    public:

      Tonemapper();
      ~Tonemapper();

      void initialize(util::SingletonManager *singletonManager);

      void doToneMapping(util::SharedPointer<db::Texture2D> combinedTexture);

    private:
      
      Tonemapper(const Tonemapper&);
      Tonemapper& operator=(const Tonemapper&);

      void createHistogramVertices();
      void createHistogram(util::SharedPointer<db::Texture2D> combinedTexture);
      void toneMapping(util::SharedPointer<db::Texture2D> combinedTexture);

      util::CPUTimer m_timer;

      util::vec2f m_logLuminanceRange, m_luminanceRange;
      util::vec2i m_logLuminanceRangeNew;

      util::SharedPointer<db::RenderShaderManager> m_renderShaderManager;

      util::SharedPointer<RenderOptions> m_options;

      util::SharedPointer<db::Texture2D> m_histogram;

      GLuint m_histogramVAO;
      GPUBuffer m_histogramVertices;
      GPUBuffer m_maxLuminanceBuffer;

      Renderquad m_histogramRenderQuad;

      util::ResourceHandle m_histogramShaderHandle;
      util::ResourceHandle m_tonemappingShaderHandle;
    };
  }
}

#endif
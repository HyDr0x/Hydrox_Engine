#ifndef FINALCOMPOSITING_H_
#define FINALCOMPOSITING_H_

#include <list>

#include <GL/glew.h>

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

    class FinalCompositing
    {
    public:

      FinalCompositing();
      ~FinalCompositing();

      void initialize(util::SingletonManager *singletonManager);

      void composeImage(util::SharedPointer<db::Texture2D> colorTexture, 
        util::SharedPointer<db::Texture2D> directlightTexture, 
        util::SharedPointer<db::Texture2D> indirectlightTexture) const;

      void renderDebugOutput(util::SharedPointer<db::Texture2D> texture) const;

      util::SharedPointer<db::Texture2D> getCombinedTexture() const;

    private:
      
      FinalCompositing(const FinalCompositing&);
      FinalCompositing& operator=(const FinalCompositing&);

      util::SharedPointer<db::RenderShaderManager> m_renderShaderManager;

      util::SharedPointer<RenderOptions> m_options;

      util::SharedPointer<db::Texture2D> m_combinedImage;

      util::ResourceHandle m_offscreenBufferShaderHandle;
      util::ResourceHandle m_combineShaderHandle;
      Renderquad m_fullscreenRenderQuad;
    };
  }
}

#endif

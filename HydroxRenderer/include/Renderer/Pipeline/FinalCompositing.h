#ifndef FINALCOMPOSITING_H_
#define FINALCOMPOSITING_H_

#include <list>

#include <GL/glew.h>

#include <DataBase/Texture2D.h>

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
    class FinalCompositing
    {
    public:

      FinalCompositing();
      ~FinalCompositing();

      void initialize(util::SingletonManager *singletonManager);

      void composeImage(util::SharedPointer<db::Texture2D> colorTexture, 
        util::SharedPointer<db::Texture2D> directlightTexture, 
        util::SharedPointer<db::Texture2D> indirectlightTexture) const;

    private:
      
      FinalCompositing(const FinalCompositing&);
      FinalCompositing& operator=(const FinalCompositing&);

      util::SingletonManager *m_singletonManager;

      util::ResourceHandle m_offscreenBufferShaderHandle;
      util::ResourceHandle m_combineShaderHandle;
      Renderquad m_fullscreenRenderQuad; 
    };
  }
}

#endif

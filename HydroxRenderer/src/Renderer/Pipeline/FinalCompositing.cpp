#include "Renderer/Pipeline/FinalCompositing.h"

#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <DataBase/RenderShader.h>
#include <DataBase/ResourceManager.hpp>

#include "Renderer/Pipeline/RenderShaderContainer.h"

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
      m_singletonManager = singletonManager;

      RenderShaderContainer *renderShader = m_singletonManager->getService<RenderShaderContainer>();
      m_offscreenBufferShaderHandle = renderShader->offscreenBufferShaderHandle;
      m_combineShaderHandle = renderShader->combineShaderHandle;
    }

    void FinalCompositing::composeImage(util::SharedPointer<db::Texture2D> colorTexture,
      util::SharedPointer<db::Texture2D> directlightTexture,
      util::SharedPointer<db::Texture2D> indirectlightTexture) const
    {
      db::RenderShader *shader = m_singletonManager->getService<db::RenderShaderManager>()->getObject(m_offscreenBufferShaderHandle);
      //db::RenderShader *shader = m_singletonManager->getService<db::RenderShaderManager>()->getObject(m_combineShaderHandle);

      shader->useShader();
      colorTexture->setTexture(0, 0);
      //directlightTexture->setTexture(1, 1);
      //indirectlightTexture->setTexture(2, 2);
      m_fullscreenRenderQuad.render();
      //indirectlightTexture->unsetTexture();
      //directlightTexture->unsetTexture();
      colorTexture->unsetTexture();
      shader->useNoShader();
    }
  }
}

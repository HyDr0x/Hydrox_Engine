#ifndef BILLBOARDRENDERER_H_
#define BILLBOARDRENDERER_H_

#include <list>

#include <GL/glew.h>

#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Signals/EventManager.h>

#include <DataBase/ResourceManager.hpp>
#include <DataBase/Texture2D.h>

#include <Shader/ShaderContainer.h>

#include <XBar/BillboardContainer.h>

#include "Renderer/Buffer/GPUBuffer.h"

namespace he
{
  namespace renderer
  {
    class BillboardRenderer
    {
    public:

      BillboardRenderer();
      ~BillboardRenderer();

      void initialize(util::SingletonManager *singletonManager);
      void render() const;

      void addRenderComponent(const xBar::BillboardContainer& billboard);

      void removeRenderComponent(const xBar::BillboardContainer& billboard);

    private:

      BillboardRenderer(const BillboardRenderer&);

      void registerRenderComponentSlots(util::SharedPointer<util::EventManager> eventManager);

      GLuint m_dummyVBO;

      sh::RenderShaderHandle m_billboardShaderHandle;

      std::list<xBar::BillboardContainer> m_renderBillboards;

      util::SharedPointer<sh::ShaderContainer> m_shaderContainer;
      util::SharedPointer<db::TextureManager> m_textureManager;
    };
  }
}

#endif

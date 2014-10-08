#ifndef BILLBOARDRENDERER_H_
#define BILLBOARDRENDERER_H_

#include <list>

#include <GL/glew.h>

#include <DataBase/ResourceManager.hpp>

#include <XBar/BillboardContainer.h>

#include "Renderer/Buffer/GPUBuffer.h"

#include <DataBase/RenderShader.h>
#include <DataBase/Texture2D.h>

#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Signals/EventManager.h>

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

      void registerRenderComponentSlots(util::EventManager *eventManager);

      GLuint m_dummyVBO;

      util::ResourceHandle m_billboardShaderHandle;

      std::list<xBar::BillboardContainer> m_renderBillboards;

      db::RenderShaderManager *m_renderShaderManager;
      db::TextureManager *m_textureManager;
    };
  }
}

#endif

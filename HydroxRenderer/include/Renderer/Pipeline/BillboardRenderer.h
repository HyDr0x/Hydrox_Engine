#ifndef BILLBOARDRENDERER_H_
#define BILLBOARDRENDERER_H_

#include <list>

#include <GL/glew.h>

#include <Renderer/Resources/ResourceManager.hpp>

#include <XBar/BillboardContainer.h>

#include "Renderer/Buffer/GPUBuffer.h"

#include "Renderer/Resources/RenderShader.h"
#include "Renderer/Resources/Texture2D.h"

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

      void initialize(util::SingletonManager *singletonManager, util::ResourceHandle billboardShaderHandle);

	    void render();

      void addRenderComponent(xBar::BillboardContainer& billboard);

      void removeRenderComponent(xBar::BillboardContainer& billboard);

    private:

	    BillboardRenderer(const BillboardRenderer&){}

      void registerRenderComponentSlots(util::EventManager *eventManager);

      GLuint m_dummyVBO;

      util::ResourceHandle m_billboardShaderHandle;

      std::list<xBar::BillboardContainer> m_renderBillboards;

	    RenderShaderManager *m_renderShaderManager;
      TextureManager *m_textureManager;
    };
	}
}

#endif

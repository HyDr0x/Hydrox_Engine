#ifndef BILLBOARDRENDERER_H_
#define BILLBOARDRENDERER_H_

#include <list>

#include <GL/glew.h>

#include <Renderer/Resources/ResourceManager.hpp>

#include "Renderer/Buffer/GPUBuffer.h"

#include "Renderer/Resources/RenderShader.h"
#include "Renderer/Resources/Texture.h"
#include "Renderer/Resources/Billboard.h"

#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Signals/EventManager.h>

#include <SceneGraph/TreeNodes/BillboardNode.h>

namespace he
{
	namespace renderer
	{
    class BillboardRenderer
    {
    public:

      BillboardRenderer();
	    ~BillboardRenderer();

      void initialize(util::SingletonManager *singletonManager, 
        util::ResourceHandle billboardShaderHandle);

	    void render();

      void addRenderComponent(sg::BillboardNode *billboardNode);
      void removeRenderComponent(sg::BillboardNode *billboardNode);

    private:

	    BillboardRenderer(const BillboardRenderer&){}

      void registerRenderComponentSlots(util::EventManager *eventManager);

      GLuint m_dummyVBO;

      util::ResourceHandle m_billboardShaderHandle;

      std::list<sg::BillboardNode*> m_renderBillboards;

	    RenderShaderManager *m_renderShaderManager;
      TextureManager *m_textureManager;
      BillboardManager *m_billboardManager;
    };
	}
}

#endif

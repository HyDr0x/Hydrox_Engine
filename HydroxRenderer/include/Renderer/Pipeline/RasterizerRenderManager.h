#ifndef RASTERIZERRENDERER_H_
#define RASTERIZERRENDERER_H_

#include <list>
#include <vector>

#include <GL/glew.h>

#include "Renderer/DLLExport.h"

#include <Renderer/Resources/ResourceManager.hpp>
#include "Renderer/Pipeline/FrustumCullingGPU.h"
#include "Renderer/Pipeline/GeometryRasterizer.h"
#include "Renderer/Pipeline/BillboardRenderer.h"
#include "Renderer/Pipeline/SpriteRenderer.h"

#include "Renderer/Buffer/UBO.h"

#include "Renderer/Resources/Mesh.h"
#include "Renderer/Resources/Material.h"
#include "Renderer/Resources/RenderShader.h"
#include "Renderer/Resources/ComputeShader.h"
#include "Renderer/Resources/Texture.h"
#include "Renderer/Resources/Billboard.h"
#include "Renderer/Resources/Sprite.h"

#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Signals/EventManager.h>

#include <SceneGraph/TreeNodes/AnimatedGeoNode.h>
#include <SceneGraph/TreeNodes/BillboardNode.h>
#include <SceneGraph/TreeNodes/GeoNode.h>
#include <SceneGraph/TreeNodes/LightNode.h>
#include <SceneGraph/TreeNodes/ParticleNode.h>

namespace he
{
	namespace renderer
	{
    class GRAPHICAPI RasterizerRenderManager : public util::SingletonBehaviour
    {
    public:

      RasterizerRenderManager();
	    ~RasterizerRenderManager();

      void setClearColor(he::util::Vector<float, 4> color);

      void resizeRenderWindow(unsigned int width, unsigned int height);

      const size_t getMaxSpriteLayer() const;

      void initialize(unsigned int maxMaterials, unsigned int maxGeometry, unsigned int maxBones, util::SingletonManager *singletonManager, GLfloat aspectRatio, size_t maxSpriteLayer, 
        util::ResourceHandle billboardShaderHandle, 
        util::ResourceHandle spriteShaderHandle, 
        util::ResourceHandle frustumCullingShaderHandle);

	    void render(util::Matrix<float, 4>& viewMatrix, util::Matrix<float, 4>& projectionMatrix, util::Vector<float, 3>& cameraPosition);

      void addSprite(util::ResourceHandle spriteID, bool transparent = true);
      void addSprite(const std::list<util::ResourceHandle>& spriteIDList, bool transparent = true);
      void removeSprite(util::ResourceHandle spriteID, bool transparent = true);
      void removeAllSprites();

      void addRenderComponent(sg::BillboardNode *billboardNode);
      void addRenderComponent(sg::GeoNode *geoNode);
      void addRenderComponent(sg::AnimatedGeoNode *animatedGeoNode);
      void addRenderComponent(sg::LightNode *lightNode);
      void addRenderComponent(sg::ParticleNode *particleNode);

      void removeRenderComponent(sg::BillboardNode *billboardNode);
      void removeRenderComponent(sg::GeoNode *geoNode);
      void removeRenderComponent(sg::AnimatedGeoNode *animatedGeoNode);
      void removeRenderComponent(sg::LightNode *lightNode);
      void removeRenderComponent(sg::ParticleNode *particleNode);

    private:

      void oldRenderPath(util::Matrix<float, 4>& viewMatrix, util::Matrix<float, 4>& projectionMatrix, util::Vector<float, 3>& cameraPosition);

	    RasterizerRenderManager(const RasterizerRenderManager&){}

      void registerRenderComponentSlots(util::EventManager *eventManager);

      UBO m_cameraParameterUBO;

      GeometryRasterizer m_geometryRasterizer;
      BillboardRenderer m_billboardRenderer;
      SpriteRenderer m_spriteRenderer;

      std::list<sg::LightNode*> m_renderLight;
      std::list<sg::ParticleNode*> m_renderParticle;

      GLfloat m_aspectRatio;
    };
	}
}

#endif

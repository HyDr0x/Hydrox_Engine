#ifndef RENDERMANAGER_H_
#define RENDERMANAGER_H_

#include <list>

#include <GL/glew.h>

#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Signals/EventManager.h>

#include "Renderer/DLLExport.h"

#include <DataBase/ResourceManager.hpp>
#include "Renderer/Pipeline/GBuffer.h"
#include "Renderer/Pipeline/LightRenderer.h"
#include "Renderer/Pipeline/ParticleRenderer.h"
#include "Renderer/Pipeline/GeometryRenderer.h"
#include "Renderer/Pipeline/BillboardRenderer.h"
#include "Renderer/Pipeline/SkyboxRenderer.h"
#include "Renderer/Pipeline/SpriteRenderer.h"
#include "Renderer/Pipeline/StringRenderer2D.h"

#include "Renderer/Buffer/UBO.h"

namespace he
{
  namespace xBar
  {
    class SkinnedGeometryContainer;
    class StaticGeometryContainer;
    class BillboardContainer;
    class LightContainer;
    class ParticleEmitterContainer;
  }

  namespace util
  {
    class SingletonManager;
  }

  namespace renderer
  {
    class RenderOptions;

    class GRAPHICAPI RenderManager : public util::SingletonBehaviour
    {
    public:

      RenderManager();
      ~RenderManager();

      void setClearColor(he::util::Vector<float, 4> color);

      void resizeRenderWindow(unsigned int width, unsigned int height);

      void setBackfaceCulling(GLenum cullingMode) const;

      void setWireframe(bool wireFrame);

      const size_t getMaxSpriteLayer() const;

      void enableSkybox(util::ResourceHandle skyboxTextureHandles[6]);
      void disableSkybox();

      void initialize(util::SingletonManager *singletonManager);

      void setViewPort(GLuint width, GLuint height, GLfloat near, GLfloat far);

      void render(util::Matrix<float, 4>& viewMatrix, util::Matrix<float, 4>& projectionMatrix, util::Vector<float, 3>& cameraPosition, float near, float far);

      void addRenderComponent(const db::Sprite* sprite);
      void addRenderComponent(const StringTexture2D* string);
      void addRenderComponent(const xBar::BillboardContainer& billboard);
      void addRenderComponent(const xBar::StaticGeometryContainer& staticGeometry);
      void addRenderComponent(const xBar::SkinnedGeometryContainer& skinnedGeometry);
      void addRenderComponent(const xBar::LightContainer& light);
      void addRenderComponent(const xBar::ParticleEmitterContainer& particleEmitter);

      void removeRenderComponent(const db::Sprite* sprite);
      void removeRenderComponent(const StringTexture2D* string);
      void removeRenderComponent(const xBar::BillboardContainer& billboard);
      void removeRenderComponent(const xBar::StaticGeometryContainer& staticGeometry);
      void removeRenderComponent(const xBar::SkinnedGeometryContainer& staticGeometry);
      void removeRenderComponent(const xBar::LightContainer& light);
      void removeRenderComponent(const xBar::ParticleEmitterContainer& particleEmitter);

    private:

      RenderManager(const RenderManager&);
      RenderManager& operator=(const RenderManager&);

      util::SingletonManager *m_singletonManager;

      UBO m_cameraParameterUBO;

      bool m_skyboxRendering;
      bool m_wireframe;

      util::ResourceHandle m_offscreenBufferShaderHandle;
      util::ResourceHandle m_combineShaderHandle;
      Renderquad m_fullscreenRenderQuad;

      RenderOptions *m_options;

      GBuffer m_gBuffer;
      LightRenderer m_lightRenderer;
      ParticleRenderer m_particleRenderer;
      GeometryRenderer m_geometryRasterizer;
      BillboardRenderer m_billboardRenderer;
      SkyboxRenderer m_skyboxRenderer;
      SpriteRenderer m_spriteRenderer;
      StringRenderer2D m_stringRenderer;

      GLfloat m_aspectRatio;
    };
  }
}

#endif

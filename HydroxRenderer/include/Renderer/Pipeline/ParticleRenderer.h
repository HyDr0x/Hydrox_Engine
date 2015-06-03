#ifndef PARTICLERENDERER_H_
#define PARTICLERENDERER_H_

#include <list>
#include <vector>

#include <GL/glew.h>

#include <DataBase/ResourceManager.hpp>

#include <Shader/ShaderContainer.h>

namespace he
{
  namespace util
  {
    class SingletonManager;
    class EventManager;
  }

  namespace xBar
  {
    class ParticleEmitterContainer;
  }

  namespace renderer
  {
    class particleEmitter;

    class ParticleRenderer
    {
    public:

      ParticleRenderer();
      ~ParticleRenderer();

      void initialize(util::SingletonManager *singletonManager);

      void render() const;

      void addRenderComponent(const xBar::ParticleEmitterContainer& particleEmitter);
      void removeRenderComponent(const xBar::ParticleEmitterContainer& particleEmitter);

    private:

      ParticleRenderer(const ParticleRenderer&);
      ParticleRenderer& operator=(const ParticleRenderer&);

      void registerRenderComponentSlots(util::SharedPointer<util::EventManager> eventManager);

      util::SharedPointer<sh::ShaderContainer> m_shaderContainer;

      sh::RenderShaderHandle m_particleShaderHandle;

      std::list<const xBar::ParticleEmitterContainer> m_particleEmitter;

      util::SharedPointer<db::ParticleEmitterManager> m_particleEmitterManager;
    };
  }
}

#endif

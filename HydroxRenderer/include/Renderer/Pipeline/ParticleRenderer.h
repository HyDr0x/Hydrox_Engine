#ifndef PARTICLERENDERER_H_
#define PARTICLERENDERER_H_

#include <list>
#include <vector>

#include <GL/glew.h>

#include <DataBase/ResourceManager.hpp>

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

      void initialize(util::SingletonManager *singletonManager, util::ResourceHandle particleShaderHandle);

      void render() const;

      void addRenderComponent(const xBar::ParticleEmitterContainer& particleEmitter);
      void removeRenderComponent(const xBar::ParticleEmitterContainer& particleEmitter);

    private:

      ParticleRenderer(const ParticleRenderer&);
      ParticleRenderer& operator=(const ParticleRenderer&);

      void registerRenderComponentSlots(util::EventManager *eventManager);

      util::ResourceHandle m_particleShaderHandle;

      std::list<const xBar::ParticleEmitterContainer> m_particleEmitter;

      db::ParticleEmitterManager *m_particleEmitterManager;
    };
  }
}

#endif

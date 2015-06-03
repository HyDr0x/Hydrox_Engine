#ifndef STRINGRENDERER2D_H_
#define STRINGRENDERER2D_H_

#include <list>
#include <vector>

#include <GL/glew.h>

#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/Signals/EventManager.h>

#include <DataBase/ResourceManager.hpp>
#include <DataBase/Texture2D.h>

#include <Shader/ShaderContainer.h>

#include "Renderer/Buffer/GPUBuffer.h"

namespace he
{
  namespace renderer
  {
    class StringTexture2D;

    class StringRenderer2D
    {
    public:

      StringRenderer2D();
      ~StringRenderer2D();

      const size_t getMaxSpriteLayer() const;

      void initialize(util::SingletonManager *singletonManager);

      void render() const;

      void addRenderComponent(const StringTexture2D* sprite);
      void removeRenderComponent(const StringTexture2D* sprite);

    private:

      StringRenderer2D(const StringRenderer2D&);
      StringRenderer2D& operator=(const StringRenderer2D&);

      void registerRenderComponentSlots(util::SharedPointer<util::EventManager> eventManager);

      sh::RenderShaderHandle m_stringShaderHandle;

      GLuint m_stringVAO;

      unsigned char m_maxLayer;

      std::list<const StringTexture2D*> m_opaqueStrings;
      std::vector<std::list<const StringTexture2D*>> m_transparentStrings;

      util::SharedPointer<sh::ShaderContainer> m_shaderContainer;
      util::SharedPointer<db::TextureManager> m_textureManager;
    };
  }
}

#endif

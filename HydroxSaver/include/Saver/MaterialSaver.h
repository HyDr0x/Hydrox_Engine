#ifndef MATERIALSAVER_H_
#define MATERIALSAVER_H_

#include <string>
#include <vector>

#include <Renderer/Resources/ResourceManager.hpp>
#include <Utilities/Math/Math.hpp>

namespace he
{
  namespace util
  {
    class SingletonManager;
  }

  namespace renderer
  {
    class Material;
  }

	namespace saver
	{
    class GRAPHICAPI MaterialSaver//could have wrong debug lines
    {
    public:

      MaterialSaver(util::SingletonManager *singletonManager);

      static void saveMaterial(std::string fileName, const renderer::Material* material);

    private:

      renderer::TextureManager *m_textureManager;
      renderer::RenderShaderManager *m_renderShaderManager;
    };
	}
}

#endif

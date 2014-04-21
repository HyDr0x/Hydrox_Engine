#ifndef STRINGTEXTURE3D_H_
#define STRINGTEXTURE3D_H_

#include <string>

#include <Utilities\Miscellaneous\ResourceHandle.h>

#include "Renderer/DLLExport.h"

namespace he
{
  namespace util
  {
    class SingletonManager;
  }

	namespace renderer
	{
    class GRAPHICAPI StringTexture3D
    {
    public:

      StringTexture3D(util::SingletonManager *singletonManager, std::string text);
      ~StringTexture3D();

      void render();

    private:

      util::ResourceHandle m_meshHandle;
      std::string m_text;
    };
	}
}

#endif

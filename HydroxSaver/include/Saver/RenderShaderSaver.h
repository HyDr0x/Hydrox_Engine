#ifndef RENDERSHADERSAVER_H_
#define RENDERSHADERSAVER_H_

#include <vector>
#include <string>
#include <map>

#include <Utilities/Miscellaneous/ResourceHandle.h>

#include "Saver/DLLExport.h"

namespace he
{
  namespace util
  {
    class SingletonManager;
  }

	namespace saver
	{
    class GRAPHICAPI RenderShaderSaver
    {
    public:

      static void save(std::string path, std::string filename, util::ResourceHandle renderShaderHandle, util::SingletonManager *singletonManager);
    };
	}
}

#endif

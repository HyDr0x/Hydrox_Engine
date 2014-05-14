#ifndef ILDEVILSAVER_H_
#define ILDEVILSAVER_H_

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
    class GRAPHICAPI ILDevilSaver
    {
    public:

      static void save(std::string path, std::string filename, util::ResourceHandle textureHandle, util::SingletonManager *singletonManager);
    };
	}
}

#endif

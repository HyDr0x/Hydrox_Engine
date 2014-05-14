#ifndef HEFSAVER_H_
#define HEFSAVER_H_

#include <vector>
#include <string>
#include <map>

#include <Utilities/Miscellaneous/ResourceHandle.h>

#include "Saver/DLLExport.h"

#include "Saver/NodeWrapper.h"
#include "Saver/NodeWrapperMapper.h"

namespace he
{
  namespace util
  {
    class SingletonManager;
  }

  namespace sg
  {
    class Scene;
  }

	namespace saver
	{
    class GRAPHICAPI HEFSaver
    {
    public:

      void save(std::string path, std::string filename, sg::Scene* scene, util::SingletonManager *singletonManager);

    private:

      NodeWrapperMapper m_wrapperMapper;
    };
	}
}

#endif

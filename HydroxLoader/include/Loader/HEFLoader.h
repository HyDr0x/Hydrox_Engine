#ifndef HEFLOADER_H_
#define HEFLOADER_H_

#include "Loader/DLLExport.h"

#include "Loader/NodeWrapper.h"
#include "Loader/NodeWrapperMapper.h"

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

  namespace loader
	{
    class GRAPHICAPI HEFLoader
    {
    public:

      sg::Scene* load(std::string path, std::string filename, util::SingletonManager *singletonManager);

    private:

      void readFromFile(std::string path, std::string filename, util::SingletonManager *singletonManager);

      loader::NodeWrapperMapper m_wrapperMapper;
    };
	}
}

#endif

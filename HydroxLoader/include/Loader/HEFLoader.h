#ifndef HEFLOADER_H_
#define HEFLOADER_H_

#include <string>

#include "Loader/DLLExport.h"

#include <Utilities/Miscellaneous/ResourceHandle.h>

#include <SceneGraph/Scene/TreeNodeAllocator.h>
#include <SceneGraph/TreeNodes/TreeNode.h>

namespace he
{  
  namespace util
  {
    class SingletonManager;
    class EventManager;
  }

  namespace sg
  {
    class Scene;
    class TreeNodeAllocator;
  }

  namespace loader
  {
    class GRAPHICAPI HEFLoader
    {
    public:

      HEFLoader(unsigned int nodeBlockSize = 256);

      sg::Scene* load(std::string path, std::string filename, util::SingletonManager *singletonManager);

    private:

      void readFromFile(std::string path, std::string filename, util::SingletonManager *singletonManager);
      void findRootNode();

      sg::NodeIndex m_rootNode;
      sg::TreeNodeAllocator m_allocator;
      std::map<std::string, std::map<unsigned int, util::ResourceHandle>> m_resourceMap;
    };
  }
}

#endif

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

      HEFLoader(float errorRate, float maxDistance, float maxAngle, unsigned int nodeBlockSize = 256);

      sg::Scene load(std::string path, std::string filename, util::SingletonManager *singletonManager);

    private:

      void readFromFile(std::string path, std::string filename, util::SingletonManager *singletonManager);
      void read(std::istream& stream, sg::TreeNodeAllocator& allocator, std::map<std::string, std::map<std::string, util::ResourceHandle>> resourceHandles, util::SharedPointer<util::EventManager> eventManager);

      float m_errorRate;
      float m_maxDistance;
      float m_maxAngle;

      sg::NodeIndex m_rootNode;
      sg::TreeNodeAllocator m_allocator;
      std::map<std::string, std::map<std::string, util::ResourceHandle>> m_resourceMap;
    };
  }
}

#endif

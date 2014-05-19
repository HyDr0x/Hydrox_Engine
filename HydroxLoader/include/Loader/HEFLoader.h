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
    class EventManager;
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

      sg::Scene* load(std::string path, std::string filename, util::SingletonManager *singletonManager, util::EventManager *eventManager);

    private:

      void readFromFile(std::string path, std::string filename, util::SingletonManager *singletonManager);
      void createSceneNodes(util::EventManager *eventManager);
      void linkSceneNodes();
      void findRootNode();
      void findNode(sg::TreeNode **treeNode, unsigned int index, NodeType nodeType);
      void findNode(sg::GroupNode **groupNode, unsigned int index, NodeType nodeType);

      loader::NodeWrapperMapper m_wrapperMapper;
      sg::GroupNode *m_rootNode;
    };
	}
}

#endif

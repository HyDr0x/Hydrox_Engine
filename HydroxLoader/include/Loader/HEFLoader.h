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
    struct NodeNumbers
    {
      unsigned int geoNodeSize;
      unsigned int animatedGeoNodeSize;
      unsigned int transformNodeSize;
      unsigned int animatedTransformNodeSize;
      unsigned int billboardNodeSize;
      unsigned int lodNodeSize;
      unsigned int lightNodeSize;
      unsigned int particleNodeSize;
    };

    struct MeshMetaData
    {
      util::Vector<float, 3> bbMax;
      util::Vector<float, 3> bbMin;
      GLuint primitiveType;
      GLuint primitiveCount;
      GLuint indexCount;
      GLuint vertexCount;
      GLuint vertexDeclaration;
      GLuint vertexStride;
      GLuint vboSize;
    };

    class GRAPHICAPI HEFLoader
    {
    public:

      ~HEFLoader();

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

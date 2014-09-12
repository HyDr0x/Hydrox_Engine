#ifndef NODEEXTRACTIONTRAVERSER_H_
#define NODEEXTRACTIONTRAVERSER_H_

#include <string>
#include <vector>

#include <DataBase/ResourceManager.hpp>
#include <SceneGraph/Traverser/ConstTraverser.h>
#include <Utilities/Math/Math.hpp>

namespace he
{
  namespace sg
  {
    class TreeNodeAllocator;

    class TransformNode;
    class AnimatedTransformNode;
    class GeoNode;
    class AnimatedGeoNode;
    class LODNode;
    class BillboardNode;
    class LightNode;
    class ShadowLightNode;
    class ParticleNode;
  }

  namespace util
  {
    class SingletonManager;
  }

  namespace saver
  {
    struct NodeWrapperMapper;

    class NodeExtractionTraverser : public sg::ConstTraverser
    {
    public:

      NodeExtractionTraverser(const sg::TreeNodeAllocator& allocator, std::string fileName, 
        std::map<std::string, std::map<util::ResourceHandle, std::string, util::Less>>& resourceMap);
      virtual ~NodeExtractionTraverser();

      virtual bool preTraverse(const sg::GeoNode& treeNode);

      virtual bool preTraverse(const sg::AnimatedGeoNode& treeNode);

      virtual bool preTraverse(const sg::BillboardNode& treeNode);

    protected:

      std::string m_fileName;

      std::map<std::string, std::map<util::ResourceHandle, std::string, util::Less>>& m_resourceMap;
    };
  }
}

#endif

#ifndef NODELINKTRAVERSER_H_
#define NODELINKTRAVERSER_H_

#include <string>
#include <vector>

#include <SceneGraph/Traverser/Traverser.h>
#include <Utilities/Math/Math.hpp>

namespace he
{
  namespace sg
  {
    class TransformNode;
    class AnimatedTransformNode;
    class GeoNode;
    class AnimatedGeoNode;
    class LODNode;
    class BillboardNode;
    class LightNode;
    class ParticleNode;
  }

  namespace saver
  {
    enum NodeType;
    struct NodeWrapperMapper;

    class NodeLinkTraverser : public sg::Traverser
    {
    public:

      NodeLinkTraverser(NodeWrapperMapper& wrapperMapper);
      virtual ~NodeLinkTraverser();

      virtual bool preTraverse(sg::TransformNode* treeNode);

      virtual bool preTraverse(sg::AnimatedTransformNode* treeNode);

      virtual bool preTraverse(sg::GeoNode* treeNode);

      virtual bool preTraverse(sg::AnimatedGeoNode* treeNode);

      virtual bool preTraverse(sg::BillboardNode* treeNode);

      virtual bool preTraverse(sg::LODNode* treeNode);

      virtual bool preTraverse(sg::LightNode* treeNode);

      virtual bool preTraverse(sg::ParticleNode* treeNode);

    protected:

      void findNodeIndex(sg::TreeNode *treeNode, unsigned int& index, NodeType& nodeType);

      std::string m_fileName;

      NodeWrapperMapper& m_wrapperMapper;
    };
  }
}

#endif

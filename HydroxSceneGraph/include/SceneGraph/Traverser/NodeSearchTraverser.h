#ifndef NODESEARCHTRAVERSER_H_
#define NODESEARCHTRAVERSER_H_

#include <string>

#include "SceneGraph/Traverser/Traverser.h"

namespace he
{
  namespace sg
  {
    class NodeSearchTraverser : public Traverser
    {
    public:

      NodeSearchTraverser(const std::string& nodeName);
      virtual ~NodeSearchTraverser();

      virtual bool preTraverse(AnimatedTransformNode* treeNode);
      virtual void postTraverse(AnimatedTransformNode* treeNode);

      virtual bool preTraverse(TransformNode* treeNode);
      virtual void postTraverse(TransformNode* treeNode);

      virtual bool preTraverse(LODNode* treeNode);
      virtual void postTraverse(LODNode* treeNode);

      virtual bool preTraverse(AnimatedGeoNode* treeNode);
      virtual void postTraverse(AnimatedGeoNode* treeNode);

      virtual bool preTraverse(GeoNode* treeNode);
      virtual void postTraverse(GeoNode* treeNode);

      virtual bool preTraverse(BillboardNode* treeNode);
      virtual void postTraverse(BillboardNode* treeNode);

      virtual bool preTraverse(ParticleNode* treeNode);
      virtual void postTraverse(ParticleNode* treeNode);

      virtual bool preTraverse(LightNode* treeNode);
      virtual void postTraverse(LightNode* treeNode);

      virtual bool preTraverse(ShadowLightNode* treeNode);
      virtual void postTraverse(ShadowLightNode* treeNode);

      TreeNode* getDiscoveredNode() const;

    protected:

      bool findNode(TreeNode *treeNode);

      TreeNode *m_discoveredNode;
      std::string m_nodeName;
    };
  }
}

#endif

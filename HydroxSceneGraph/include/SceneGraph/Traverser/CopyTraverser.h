#ifndef COPYTRAVERSER_H_
#define COPYTRAVERSER_H_

#include <map>
#include <list>
#include <string>

#include "SceneGraph/Traverser/ConstTraverser.h"

namespace he
{
  namespace sg
  {
    class CopyTraverser : public ConstTraverser
    {
    public:

      CopyTraverser(const TreeNodeAllocator& sourceAllocator, TreeNodeAllocator& destinationAllocator, std::string namePrefix);
      virtual ~CopyTraverser();

      virtual bool preTraverse(const AnimatedTransformNode& treeNode);
      virtual void postTraverse(const AnimatedTransformNode& treeNode);

      virtual bool preTraverse(const TransformNode& treeNode);
      virtual void postTraverse(const TransformNode& treeNode);

      virtual bool preTraverse(const LODNode& treeNode);
      virtual void postTraverse(const LODNode& treeNode);

      virtual bool preTraverse(const AnimatedGeoNode& treeNode);
      virtual void postTraverse(const AnimatedGeoNode& treeNode);

      virtual bool preTraverse(const GeoNode& treeNode);
      virtual void postTraverse(const GeoNode& treeNode);

      virtual bool preTraverse(const BillboardNode& treeNode);
      virtual void postTraverse(const BillboardNode& treeNode);

      virtual bool preTraverse(const ParticleNode& treeNode);
      virtual void postTraverse(const ParticleNode& treeNode);

      virtual bool preTraverse(const LightNode& treeNode);
      virtual void postTraverse(const LightNode& treeNode);

      virtual bool preTraverse(const ShadowLightNode& treeNode);
      virtual void postTraverse(const ShadowLightNode& treeNode);

      NodeIndex getCopiedRootNode() const;

    protected:

      NodeIndex cloneGroupNode(const GroupNode& treeNode);
      NodeIndex cloneTreeNode(const TreeNode& treeNode);
      void ascendingGroupNode(const GroupNode& treeNode);
      void ascendingTreeNode(const TreeNode& treeNode);

      void addChild(NodeIndex parentIndex, TreeNode& newNode);
      void addSibling(NodeIndex parentIndex, TreeNode& newNode);

      TreeNodeAllocator &m_destinationAllocator;

      std::map<const AnimatedGeoNode*, std::list<AnimatedTransformNode*>> m_oldGeoNodeTable;//Key: old geonode | Value: all new animatedTransformNodes which reference the new geonode
      std::map<const AnimatedGeoNode*, NodeIndex> m_newGeoNodeTable;//Key: old geoNode | Value: new geoNode

      //std::map<const AnimatedGeoNode&, std::list<AnimatedTransformNode&>> m_oldGeoNodeTable;
      //std::map<const AnimatedGeoNode&, AnimatedGeoNode&> m_newGeoNodeTable;

      std::string m_namePrefix;

      NodeIndex m_rootNode;
      NodeIndex m_parent;
      NodeIndex m_sibling;

      bool m_childNode;
    };
  }
}

#endif

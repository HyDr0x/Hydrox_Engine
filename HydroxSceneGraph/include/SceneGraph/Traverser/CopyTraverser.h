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

      CopyTraverser(std::string namePrefix);
      virtual ~CopyTraverser();

      virtual bool preTraverse(const AnimatedTransformNode* treeNode);
      virtual void postTraverse(const AnimatedTransformNode* treeNode);

      virtual bool preTraverse(const TransformNode* treeNode);
      virtual void postTraverse(const TransformNode* treeNode);

      virtual bool preTraverse(const LODNode* treeNode);
      virtual void postTraverse(const LODNode* treeNode);

      virtual bool preTraverse(const AnimatedGeoNode* treeNode);
      virtual void postTraverse(const AnimatedGeoNode* treeNode);

      virtual bool preTraverse(const GeoNode* treeNode);
      virtual void postTraverse(const GeoNode* treeNode);

      virtual bool preTraverse(const BillboardNode* treeNode);
      virtual void postTraverse(const BillboardNode* treeNode);

      virtual bool preTraverse(const ParticleNode* treeNode);
      virtual void postTraverse(const ParticleNode* treeNode);

      virtual bool preTraverse(const LightNode* treeNode);
      virtual void postTraverse(const LightNode* treeNode);

      GroupNode* getCopiedRootNode() const;

    protected:

      GroupNode* cloneGroupNode(const GroupNode* treeNode);
      TreeNode* cloneTreeNode(const TreeNode* treeNode);
      void ascendingGroupNode(const GroupNode* treeNode);
      void ascendingTreeNode(const TreeNode* treeNode);

      void addChild(GroupNode* parent, TreeNode* newNode);
      void addSibling(TreeNode* sibling, TreeNode* newNode);

      std::map<const AnimatedGeoNode*, std::list<AnimatedTransformNode*>> m_oldGeoNodeTable;
      std::map<const AnimatedGeoNode*, AnimatedGeoNode*> m_newGeoNodeTable;

      std::string m_namePrefix;

      GroupNode* m_rootNode;
      GroupNode* m_parent;
      TreeNode* m_sibling;

      bool m_childNode;
    };
  }
}

#endif

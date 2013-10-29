#ifndef COPYTRAVERSER_H_
#define COPYTRAVERSER_H_

#include <string>

#include "Hydrox/DLLExport.h"

#include "Hydrox/Utility/Traverser/Traverser.h"

class GRAPHICAPI CopyTraverser : public Traverser
{
public:

  CopyTraverser(std::string namePrefix);
  virtual ~CopyTraverser();

  void doTraverse(TreeNode* treeNode);

  virtual bool preTraverse(TransformNode* treeNode);
  virtual void postTraverse(TransformNode* treeNode);

  virtual bool preTraverse(LODNode* treeNode);
  virtual void postTraverse(LODNode* treeNode);

  virtual bool preTraverse(GeoNode* treeNode);
  virtual void postTraverse(GeoNode* treeNode);

  virtual bool preTraverse(BillboardNode* treeNode);
  virtual void postTraverse(BillboardNode* treeNode);

  virtual bool preTraverse(ParticleNode* treeNode);
  virtual void postTraverse(ParticleNode* treeNode);

  virtual bool preTraverse(LightNode* treeNode);
  virtual void postTraverse(LightNode* treeNode);

  GroupNode* getCopiedRootNode();

protected:

  bool cloneGroupNode(GroupNode* treeNode);
  bool cloneTreeNode(TreeNode* treeNode);
  void ascendingGroupNode(TreeNode* treeNode);
  void ascendingTreeNode(TreeNode* treeNode);

  void addChild(GroupNode* parent, TreeNode* newNode);
  void addSibling(TreeNode* sibling, TreeNode* newNode);

  std::string m_namePrefix;

  GroupNode* m_rootNode;
  GroupNode* m_parent;
  TreeNode* m_sibling;

  bool m_childNode;
};

#endif

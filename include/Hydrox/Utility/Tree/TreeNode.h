#ifndef TREENODE_H_
#define TREENODE_H_

#include <string>

#include "Hydrox/DLLExport.h"

class Traverser;

class GroupNode;

class GRAPHICAPI TreeNode
{
public:

  TreeNode(const std::string& nodeName, GroupNode* parent = nullptr, TreeNode* nextSibling = nullptr);
  virtual TreeNode& operator=(const TreeNode& sourceNode);
  virtual ~TreeNode() = 0;

  virtual TreeNode* clone() const = 0;

  virtual bool ascendTraverse(Traverser* traverser) = 0;
  virtual bool preTraverse(Traverser* traverser) = 0;
  virtual void postTraverse(Traverser* traverser) = 0;

  virtual TreeNode* getFirstChild() const;
  TreeNode* getNextSibling() const;
  GroupNode* getParent() const;

  void setNextSibling(TreeNode* nextSibling);
  void setParent(GroupNode* parent);

  void setNodeName(std::string& nodeName);
  const std::string& getNodeName();

protected:

  std::string m_nodeName;

  GroupNode* m_parent;
  TreeNode* m_nextSibling;

private:

  TreeNode(const TreeNode&){}
};

#endif

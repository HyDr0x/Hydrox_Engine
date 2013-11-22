#ifndef BILLBOARDNODE_H_
#define BILLBOARDNODE_H_

#include "Hydrox/DLLExport.h"

#include "Hydrox/Graphics/Billboard.h"
#include "Hydrox/Utility/Tree/TreeNode.h"

class Traverser;


class GRAPHICAPI BillboardNode : public TreeNode
{
public:

  BillboardNode(ResourceHandle billboardIndex, bool renderable, const std::string& nodeName, GroupNode* parent = nullptr, TreeNode* nextSibling = nullptr);
  BillboardNode& operator=(const BillboardNode& sourceNode);
  virtual TreeNode& operator=(const TreeNode& sourceNode);
  virtual ~BillboardNode();

  virtual TreeNode* clone() const;

  virtual bool ascendTraverse(Traverser* traverser);
  virtual bool preTraverse(Traverser* traverser);
  virtual void postTraverse(Traverser* traverser);

  ResourceHandle getBillboardIndex() const;

  Matrix<float, 4> getTransformationMatrix() const;
  void setTransformationMatrix(const Matrix<float, 4>& trfMatrix);

  void setRenderable(bool renderable);
	bool getRenderable() const;

  void setDeleteThis(bool deleteThis);
  bool getDeleteThis();

private:

  Matrix<float,4> m_trfMatrix;//the tranformation Matrix

  ResourceHandle m_billboardIndex;//index which points into the array of the texturemanager

  bool m_renderable;//boolean which decides if the mesh is being drawn or not

  bool m_deleteThis;
};

#endif

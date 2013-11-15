#ifndef LIGHTNODE_H_
#define LIGHTNODE_H_

#include "Hydrox/DLLExport.h"

#include "Hydrox/Utility/Math/Math.hpp"
#include "Hydrox/Graphics/Light.h"
#include "Hydrox/Utility/Tree/TreeNode.h"

class Traverser;

class GRAPHICAPI LightNode : public TreeNode
{
public:

  LightNode(const std::string& nodeName, GroupNode* parent = nullptr, TreeNode* nextSibling = nullptr);
  LightNode& operator=(const LightNode& destinationNode);
  virtual TreeNode& operator=(const TreeNode& destinationNode);
  virtual ~LightNode();

  virtual TreeNode* clone() const;

  virtual bool ascendTraverse(Traverser* traverser);
  virtual bool preTraverse(Traverser* traverser);
  virtual void postTraverse(Traverser* traverser);

  Matrix<float, 4> getTransformationMatrix() const;
  void setTransformationMatrix(const Matrix<float, 4>& trfMatrix);

private:

  Matrix<float,4> m_trfMatrix;//the tranformation Matrix
};

#endif

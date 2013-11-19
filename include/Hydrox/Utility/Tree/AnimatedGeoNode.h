#ifndef ANIMATEDGEONODE_H_
#define ANIMATEDGEONODE_H_

#include <vector>

#include "Hydrox/DLLExport.h"

#include "Hydrox/Graphics/Mesh.h"
#include "Hydrox/Utility/Tree/TreeNode.h"

class Traverser;

class GRAPHICAPI AnimatedGeoNode : public TreeNode
{
public:

  AnimatedGeoNode(ResourceHandle meshIndex, bool renderable, const std::string& nodeName, GroupNode* parent = nullptr, TreeNode* nextSibling = nullptr);
  AnimatedGeoNode& operator=(const AnimatedGeoNode& destinationNode);
  virtual TreeNode& operator=(const TreeNode& destinationNode);
  virtual ~AnimatedGeoNode();

  virtual TreeNode* clone() const;

  virtual bool ascendTraverse(Traverser* traverser);
  virtual bool preTraverse(Traverser* traverser);
  virtual void postTraverse(Traverser* traverser);

  ResourceHandle getMeshIndex() const;

  Matrix<float, 4> getTransformationMatrix() const;
  void setTransformationMatrix(const Matrix<float, 4>& trfMatrix);

  void setRenderable(bool renderable);
	bool getRenderable() const;

  void setDeleteThis(bool deleteThis);
  bool getDeleteThis();

private:

  Matrix<float,4> m_trfMatrix;//the tranformation Matrix

  std::vector<Matrix<float, 4>> m_inverseBindPoseMatrices;
  std::vector<Matrix<float, 4>> m_boneTransformMatrices;

  ResourceHandle m_meshIndex;//index which points into the array of the modelmanager

  bool m_renderable;//boolean which decides if the mesh is being drawn or not

  bool m_deleteThis;
};

#endif

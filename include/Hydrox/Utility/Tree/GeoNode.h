#ifndef GEONODE_H_
#define GEONODE_H_

#include "Hydrox/DLLExport.h"

#include "Hydrox/Graphics/Mesh.h"
#include "Hydrox/Utility/Tree/TreeNode.h"

class Traverser;


class GRAPHICAPI GeoNode : public TreeNode
{
public:

  GeoNode(ResourceHandle meshIndex, bool renderable, const std::string& nodeName, GroupNode* parent = nullptr, TreeNode* nextSibling = nullptr);
  GeoNode& operator=(const GeoNode& destinationNode);
  virtual TreeNode& operator=(const TreeNode& destinationNode);
  virtual ~GeoNode();

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

  ResourceHandle m_meshIndex;//index which points into the array of the modelmanager

  bool m_renderable;//boolean which decides if the mesh is being drawn or not

  bool m_deleteThis;
};

#endif

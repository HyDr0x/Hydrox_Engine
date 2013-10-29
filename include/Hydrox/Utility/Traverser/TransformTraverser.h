#ifndef TRANSFORMTRAVERSER_H_
#define TRANSFORMTRAVERSER_H_

#include <stack>

#include "Hydrox/DLLExport.h"

#include "Hydrox/Utility/Traverser/Traverser.h"
#include "Hydrox/Utility/Math/Math.hpp"

class GRAPHICAPI TransformTraverser : public Traverser
{
public:

  TransformTraverser();
  virtual ~TransformTraverser();

  void insertTransformMatrix(Mat<float, 4>& trfMatrix);

  std::stack<Mat<float, 4>>& getMatrixStack();

  virtual bool ascendTraverse(TransformNode* treeNode);
  virtual bool preTraverse(TransformNode* treeNode);
  virtual void postTraverse(TransformNode* treeNode);

  virtual bool ascendTraverse(LODNode* treeNode);
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

protected:

  std::stack<Mat<float, 4>> m_matrixStack;
};

#endif

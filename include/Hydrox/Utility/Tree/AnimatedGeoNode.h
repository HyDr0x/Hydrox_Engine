#ifndef ANIMATEDGEONODE_H_
#define ANIMATEDGEONODE_H_

#include <vector>

#include "Hydrox/DLLExport.h"

#include "Hydrox/Graphics/Mesh.h"
#include "Hydrox/Utility/Tree/GeoNode.h"

class Traverser;

class GRAPHICAPI AnimatedGeoNode : public GeoNode
{
public:

  AnimatedGeoNode(const std::vector<Matrix<float, 4>>& inverseBindPoseMatrices, ResourceHandle meshIndex, bool renderable, const std::string& nodeName, GroupNode* parent = nullptr, TreeNode* nextSibling = nullptr);
  AnimatedGeoNode& operator=(const AnimatedGeoNode& sourceNode);
  virtual TreeNode& operator=(const TreeNode& sourceNode);
  virtual ~AnimatedGeoNode();

  virtual TreeNode* clone() const;

  virtual bool ascendTraverse(Traverser* traverser);
  virtual bool preTraverse(Traverser* traverser);
  virtual void postTraverse(Traverser* traverser);

  void setBoneTransform(const Matrix<float, 4>& boneTransform, unsigned int boneIndex);

  std::vector<Matrix<float, 4>> getSkinningMatrices();

private:

  std::vector<Matrix<float, 4>> m_inverseBindPoseMatrices;
  std::vector<Matrix<float, 4>> m_boneTransformMatrices;
};

#endif

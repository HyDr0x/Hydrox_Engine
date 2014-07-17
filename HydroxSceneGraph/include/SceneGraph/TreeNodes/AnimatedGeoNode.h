#ifndef ANIMATEDGEONODE_H_
#define ANIMATEDGEONODE_H_

#include <vector>

#include "SceneGraph/DLLExport.h"

#include <Utilities/Miscellaneous/ResourceHandle.h>
#include "SceneGraph/TreeNodes/GeoNode.h"

namespace he
{
  namespace sg
  {    class GRAPHICAPI AnimatedGeoNode : public GeoNode
    {
    public:

      AnimatedGeoNode(std::istream stream);
      AnimatedGeoNode(const std::vector<util::Matrix<float, 4>>& inverseBindPoseMatrices, util::EventManager *eventManager, util::ResourceHandle meshHandle, util::ResourceHandle materialHandle, 
        const std::string& nodeName, NodeIndex parent = ~0, NodeIndex nextSibling = ~0);
      AnimatedGeoNode(const TreeNode& sourceNode);
      virtual ~AnimatedGeoNode();

      virtual TreeNode& operator=(const TreeNode& sourceNode);

      virtual TreeNode* clone() const;

      virtual bool ascendTraverse(Traverser* traverser);
      virtual bool preTraverse(Traverser* traverser);
      virtual void postTraverse(Traverser* traverser);

      virtual bool ascendTraverse(ConstTraverser* traverser) const;
      virtual bool preTraverse(ConstTraverser* traverser) const;
      virtual void postTraverse(ConstTraverser* traverser) const;

      void setMeshHandle(util::ResourceHandle meshHandle);
      void setMaterialHandle(util::ResourceHandle materialHandle);
      void setRenderable(bool renderable);

      void setBoneTransform(const util::Matrix<float, 4>& boneTransform, unsigned int boneIndex);

      std::vector<util::Matrix<float, 4>> getSkinningMatrices() const;
      std::vector<util::Matrix<float, 4>> getInverseBindPoseMatrices() const;

    private:

      std::vector<util::Matrix<float, 4>> m_inverseBindPoseMatrices;
      std::vector<util::Matrix<float, 4>> m_boneTransformMatrices;
    };
  }
}

#endif

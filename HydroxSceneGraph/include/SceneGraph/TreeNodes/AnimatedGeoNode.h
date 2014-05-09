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

      AnimatedGeoNode(const std::vector<util::Matrix<float, 4>>& inverseBindPoseMatrices, util::EventManager *eventManager, util::ResourceHandle meshHandle, util::ResourceHandle materialHandle, bool transparency, const std::string& nodeName, GroupNode* parent = nullptr, TreeNode* nextSibling = nullptr);
      virtual ~AnimatedGeoNode();

      AnimatedGeoNode& operator=(const AnimatedGeoNode& sourceNode);
      virtual TreeNode& operator=(const TreeNode& sourceNode);

      virtual TreeNode* clone() const;

      virtual bool ascendTraverse(Traverser* traverser);
      virtual bool preTraverse(Traverser* traverser);
      virtual void postTraverse(Traverser* traverser);

      virtual bool isAnimatedGeoNode() const;

      void setMeshHandle(util::ResourceHandle meshHandle);
      void setMaterialHandle(util::ResourceHandle materialHandle);
      void setRenderable(bool renderable);

      void setBoneTransform(const util::Matrix<float, 4>& boneTransform, unsigned int boneIndex);

      std::vector<util::Matrix<float, 4>> getSkinningMatrices() const;

    private:

      std::vector<util::Matrix<float, 4>> m_inverseBindPoseMatrices;
      std::vector<util::Matrix<float, 4>> m_boneTransformMatrices;
    };
	}
}

#endif

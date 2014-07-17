#ifndef ADDNODESTRAVERSER_H_
#define ADDNODESTRAVERSER_H_

#include <vector>

#include "SceneGraph/Traverser/Traverser.h"
#include <Utilities/Math/Math.hpp>
#include <Utilities/Signals/EventManager.h>

namespace he
{
  namespace sg
  {
    class AddNodesTraverser : public Traverser
    {
    public:

      AddNodesTraverser(TreeNodeAllocator& allocator, util::EventManager& eventManager, std::vector<float> lodRanges, util::Vector<float, 3> camPos);
      virtual ~AddNodesTraverser();

      virtual bool preTraverse(AnimatedTransformNode& treeNode);
      virtual void postTraverse(AnimatedTransformNode& treeNode);

      virtual bool preTraverse(LODNode& treeNode);
      virtual void postTraverse(LODNode& treeNode);

      virtual bool preTraverse(AnimatedGeoNode& treeNode);
      virtual void postTraverse(AnimatedGeoNode& treeNode);

      virtual bool preTraverse(GeoNode& treeNode);
      virtual void postTraverse(GeoNode& treeNode);

      virtual bool preTraverse(BillboardNode& treeNode);
      virtual void postTraverse(BillboardNode& treeNode);

      virtual bool preTraverse(ParticleNode& treeNode);
      virtual void postTraverse(ParticleNode& treeNode);

      virtual bool preTraverse(LightNode& treeNode);
      virtual void postTraverse(LightNode& treeNode);

      virtual bool preTraverse(ShadowLightNode& treeNode);
      virtual void postTraverse(ShadowLightNode& treeNode);

    protected:

      util::EventManager& m_eventManager;

      util::Vector<float, 3> m_camPos;
      std::vector<float> m_lodRanges;
    };
  }
}

#endif

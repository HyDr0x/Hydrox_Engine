#ifndef REMOVENODESTRAVERSER_H_
#define REMOVENODESTRAVERSER_H_

#include <list>
#include <vector>

#include <Utilities/Math/Math.hpp>
#include <Utilities/Signals/EventManager.h>

#include "SceneGraph/Traverser/Traverser.h"

namespace he
{
  namespace sg
  {    class RemoveNodesTraverser : public Traverser
    {
    public:

      RemoveNodesTraverser(TreeNodeAllocator& allocator, util::SharedPointer<util::EventManager> eventManager);
      virtual ~RemoveNodesTraverser();

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

      util::SharedPointer<util::EventManager> m_eventManager;
    };
  }
}

#endif

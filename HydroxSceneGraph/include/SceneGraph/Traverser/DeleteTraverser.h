#ifndef DELETETRAVERSER_H_
#define DELETETRAVERSER_H_

#include "SceneGraph/Traverser/Traverser.h"

namespace he
{
  namespace sg
  {
    class DeleteTraverser : public Traverser
    {
    public:

      DeleteTraverser(TreeNodeAllocator& allocator);
      virtual ~DeleteTraverser();

      virtual void postTraverse(AnimatedTransformNode& treeNode);

      virtual void postTraverse(TransformNode& treeNode);

      virtual void postTraverse(LODNode& treeNode);

      virtual void postTraverse(AnimatedGeoNode& treeNode);

      virtual void postTraverse(GeoNode& treeNode);

      virtual void postTraverse(BillboardNode& treeNode);

      virtual void postTraverse(ParticleNode& treeNode);

      virtual void postTraverse(LightNode& treeNode);

      virtual void postTraverse(ShadowLightNode& treeNode);
    };
  }
}

#endif

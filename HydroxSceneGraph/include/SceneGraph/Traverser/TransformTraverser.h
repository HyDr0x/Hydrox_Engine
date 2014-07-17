#ifndef TRANSFORMTRAVERSER_H_
#define TRANSFORMTRAVERSER_H_

#include <stack>

#include "SceneGraph/Traverser/Traverser.h"
#include <Utilities/Math/Math.hpp>

namespace he
{
  namespace sg
  {
    class TransformTraverser : public Traverser
    {
    public:

      TransformTraverser(TreeNodeAllocator& allocator);
      virtual ~TransformTraverser();

      void clearStacks();

      virtual bool ascendTraverse(AnimatedTransformNode& treeNode);
      virtual bool preTraverse(AnimatedTransformNode& treeNode);
      virtual void postTraverse(AnimatedTransformNode& treeNode);

      virtual bool ascendTraverse(TransformNode& treeNode);
      virtual bool preTraverse(TransformNode& treeNode);
      virtual void postTraverse(TransformNode& treeNode);

      virtual bool ascendTraverse(LODNode& treeNode);
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

      virtual void postAscendTraverse();

      std::stack<float> m_scaleStack;
      std::stack<util::Vector<float, 3>> m_translateStack;
      std::stack<util::Quaternion<float>> m_rotationStack;
    };
  }
}

#endif

#ifndef DELETETRAVERSER_H_
#define DELETETRAVERSER_H_

#include "SceneGraph/Traverser/ConstTraverser.h"

namespace he
{
  namespace sg
  {
    class DeleteTraverser : public ConstTraverser
    {
    public:

      DeleteTraverser();
      virtual ~DeleteTraverser();

      virtual bool preTraverse(const AnimatedTransformNode* treeNode);
      virtual void postTraverse(const AnimatedTransformNode* treeNode);

      virtual bool preTraverse(const TransformNode* treeNode);
      virtual void postTraverse(const TransformNode* treeNode);

      virtual bool preTraverse(const LODNode* treeNode);
      virtual void postTraverse(const LODNode* treeNode);

      virtual bool preTraverse(const AnimatedGeoNode* treeNode);
      virtual void postTraverse(const AnimatedGeoNode* treeNode);

      virtual bool preTraverse(const GeoNode* treeNode);
      virtual void postTraverse(const GeoNode* treeNode);

      virtual bool preTraverse(const BillboardNode* treeNode);
      virtual void postTraverse(const BillboardNode* treeNode);

      virtual bool preTraverse(const ParticleNode* treeNode);
      virtual void postTraverse(const ParticleNode* treeNode);

      virtual bool preTraverse(const LightNode* treeNode);
      virtual void postTraverse(const LightNode* treeNode);

      virtual bool preTraverse(const ShadowLightNode* treeNode);
      virtual void postTraverse(const ShadowLightNode* treeNode);
    };
  }
}

#endif

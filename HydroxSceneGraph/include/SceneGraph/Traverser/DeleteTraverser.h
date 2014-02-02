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

      DeleteTraverser();
      virtual ~DeleteTraverser();

      virtual bool preTraverse(AnimatedTransformNode* treeNode);
      virtual void postTraverse(AnimatedTransformNode* treeNode);

      virtual bool preTraverse(TransformNode* treeNode);
      virtual void postTraverse(TransformNode* treeNode);

      virtual bool preTraverse(LODNode* treeNode);
      virtual void postTraverse(LODNode* treeNode);

      virtual bool preTraverse(AnimatedGeoNode* treeNode);
      virtual void postTraverse(AnimatedGeoNode* treeNode);

      virtual bool preTraverse(GeoNode* treeNode);
      virtual void postTraverse(GeoNode* treeNode);

      virtual bool preTraverse(BillboardNode* treeNode);
      virtual void postTraverse(BillboardNode* treeNode);

      virtual bool preTraverse(ParticleNode* treeNode);
      virtual void postTraverse(ParticleNode* treeNode);

      virtual bool preTraverse(LightNode* treeNode);
      virtual void postTraverse(LightNode* treeNode);

    protected:

      TreeNode *m_lastVisitedNode;
      bool m_postKill;

    };
	}
}

#endif

#ifndef CONSTTRAVERSER_H_
#define CONSTTRAVERSER_H_

#include "SceneGraph/DLLExport.h"

namespace he
{
	namespace sg
	{
    class TreeNode;
    class GroupNode;

    class LODNode;
    class AnimatedTransformNode;
    class TransformNode;
    class AnimatedGeoNode;
    class GeoNode;
    class BillboardNode;
    class ParticleNode;
    class LightNode;

    class GRAPHICAPI ConstTraverser
    {
    public:

      ConstTraverser();
      virtual ~ConstTraverser() = 0;

      virtual void doAscend(const TreeNode *treeNode);//starts with the parent node of the given treeNode

      virtual void doTraverse(const TreeNode *treeNode);//starts with the given tree node, but without its siblings

      virtual bool ascendTraverse(const TreeNode* treeNode);
      virtual bool preTraverse(const TreeNode* treeNode);
      virtual void postTraverse(const TreeNode* treeNode);

      virtual bool ascendTraverse(const GroupNode* treeNode);
      virtual bool preTraverse(const GroupNode* treeNode);
      virtual void postTraverse(const GroupNode* treeNode);

      virtual bool ascendTraverse(const AnimatedTransformNode* treeNode);
      virtual bool preTraverse(const AnimatedTransformNode* treeNode);
      virtual void postTraverse(const AnimatedTransformNode* treeNode);

      virtual bool ascendTraverse(const TransformNode* treeNode);
      virtual bool preTraverse(const TransformNode* treeNode);
      virtual void postTraverse(const TransformNode* treeNode);

      virtual bool ascendTraverse(const LODNode* treeNode);
      virtual bool preTraverse(const LODNode* treeNode);
      virtual void postTraverse(const LODNode* treeNode);

      virtual bool ascendTraverse(const AnimatedGeoNode* treeNode);
      virtual bool preTraverse(const AnimatedGeoNode* treeNode);
      virtual void postTraverse(const AnimatedGeoNode* treeNode);

      virtual bool ascendTraverse(const GeoNode* treeNode);
      virtual bool preTraverse(const GeoNode* treeNode);
      virtual void postTraverse(const GeoNode* treeNode);

      virtual bool ascendTraverse(const BillboardNode* treeNode);
      virtual bool preTraverse(const BillboardNode* treeNode);
      virtual void postTraverse(const BillboardNode* treeNode);

      virtual bool ascendTraverse(const ParticleNode* treeNode);
      virtual bool preTraverse(const ParticleNode* treeNode);
      virtual void postTraverse(const ParticleNode* treeNode);

      virtual bool ascendTraverse(const LightNode* treeNode);
      virtual bool preTraverse(const LightNode* treeNode);
      virtual void postTraverse(const LightNode* treeNode);

    protected:

      void doTraverseDown(const TreeNode *treeNode);

      virtual void postAscendTraverse();

      bool m_stopTraversal;
    };
	}
}

#endif

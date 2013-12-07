#ifndef TRAVERSER_H_
#define TRAVERSER_H_

namespace he
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

  class Traverser
  {
  public:

    enum TraverserFlags
    {
      TRAVERSE_DEFAULT = 0,
      TRAVERSE_IGNORE_LODS = 1,
    };

    Traverser();
    virtual ~Traverser() = 0;

    void addTraverserFlag(TraverserFlags traverserFlag);
    void removeTraverserFlag(TraverserFlags traverserFlag);
    TraverserFlags getTraverserFlag() const;

    virtual void doAscend(TreeNode* treeNode);//starts with the parent node of the given treeNode

    virtual void doTraverse(TreeNode* treeNode);//starts with the given tree node, but without its siblings

    virtual bool ascendTraverse(TreeNode* treeNode);
    virtual bool preTraverse(TreeNode* treeNode);
    virtual void postTraverse(TreeNode* treeNode);

    virtual bool ascendTraverse(GroupNode* treeNode);
    virtual bool preTraverse(GroupNode* treeNode);
    virtual void postTraverse(GroupNode* treeNode);

    virtual bool ascendTraverse(AnimatedTransformNode* treeNode);
    virtual bool preTraverse(AnimatedTransformNode* treeNode);
    virtual void postTraverse(AnimatedTransformNode* treeNode);

    virtual bool ascendTraverse(TransformNode* treeNode);
    virtual bool preTraverse(TransformNode* treeNode);
    virtual void postTraverse(TransformNode* treeNode);

    virtual bool ascendTraverse(LODNode* treeNode);
    virtual bool preTraverse(LODNode* treeNode);
    virtual void postTraverse(LODNode* treeNode);

    virtual bool ascendTraverse(AnimatedGeoNode* treeNode);
    virtual bool preTraverse(AnimatedGeoNode* treeNode);
    virtual void postTraverse(AnimatedGeoNode* treeNode);

    virtual bool ascendTraverse(GeoNode* treeNode);
    virtual bool preTraverse(GeoNode* treeNode);
    virtual void postTraverse(GeoNode* treeNode);

    virtual bool ascendTraverse(BillboardNode* treeNode);
    virtual bool preTraverse(BillboardNode* treeNode);
    virtual void postTraverse(BillboardNode* treeNode);

    virtual bool ascendTraverse(ParticleNode* treeNode);
    virtual bool preTraverse(ParticleNode* treeNode);
    virtual void postTraverse(ParticleNode* treeNode);

    virtual bool ascendTraverse(LightNode* treeNode);
    virtual bool preTraverse(LightNode* treeNode);
    virtual void postTraverse(LightNode* treeNode);

  protected:

    void doTraverseDown(TreeNode* treeNode);//starts with the given treeNode and traverses its siblings too

    virtual void postAscendTraverse();

    bool m_stopTraversal;
    unsigned int m_traverserFlags;
  };
}

#endif

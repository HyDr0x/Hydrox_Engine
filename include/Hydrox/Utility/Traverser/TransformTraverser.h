#ifndef TRANSFORMTRAVERSER_H_
#define TRANSFORMTRAVERSER_H_

#include <stack>

#include "Hydrox/Utility/Traverser/Traverser.h"
#include "Hydrox/Utility/Math/Math.hpp"

namespace he
{
  class TransformTraverser : public Traverser
  {
  public:

    TransformTraverser();
    virtual ~TransformTraverser();

    void clearStacks();

    virtual bool ascendTraverse(AnimatedTransformNode* treeNode);
    virtual bool preTraverse(AnimatedTransformNode* treeNode);
    virtual void postTraverse(AnimatedTransformNode* treeNode);

    virtual bool ascendTraverse(TransformNode* treeNode);
    virtual bool preTraverse(TransformNode* treeNode);
    virtual void postTraverse(TransformNode* treeNode);

    virtual bool ascendTraverse(LODNode* treeNode);
    virtual bool preTraverse(LODNode* treeNode);
    virtual void postTraverse(LODNode* treeNode);

    virtual bool preTraverse(GeoNode* treeNode);
    virtual void postTraverse(GeoNode* treeNode);

    virtual bool preTraverse(BillboardNode* treeNode);
    virtual void postTraverse(BillboardNode* treeNode);

    virtual bool preTraverse(ParticleNode* treeNode);
    virtual void postTraverse(ParticleNode* treeNode);

    virtual bool preTraverse(LightNode* treeNode);
    virtual void postTraverse(LightNode* treeNode);

  protected:

    virtual void postAscendTraverse();

    std::stack<float> m_scaleStack;
    std::stack<Vector<float, 3>> m_translateStack;
    std::stack<Quaternion<float>> m_rotationStack;
  };
}

#endif

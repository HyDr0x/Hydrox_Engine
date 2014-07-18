#include "SceneGraph/Traverser/Traverser.h"

#include "SceneGraph/TreeNodes/TreeNode.h"
#include "SceneGraph/TreeNodes/GroupNode.h"

#include "SceneGraph/Scene/Scene.h"

namespace he
{
  namespace sg
  {
    Traverser::Traverser(TreeNodeAllocator& allocator) : m_allocator(allocator), m_stopTraversal(false)
    {
    }

    Traverser::~Traverser()
    {
    }

    void Traverser::doAscend(TreeNode& treeNode)
    {
      NodeIndex index = treeNode.getParent();
      while(index != ~0)//start calculating the trfMatrix of the upper path of the actual node
      {
        TreeNode& treeNode2 = m_allocator[index];

        if(!treeNode2.ascendTraverse(this) || m_stopTraversal)
        {
          return;
        }

        index = treeNode2.getParent();
      }

      postAscendTraverse();
    }

    void Traverser::doTraverse(TreeNode &treeNode)
    {
      if(treeNode.preTraverse(this))
      {
        doTraverseDown(treeNode.getFirstChild());
      }
      treeNode.postTraverse(this);
    }

    void Traverser::doTraverseDown(NodeIndex nodeIndex)
    {
      while(nodeIndex != ~0)
      {
        TreeNode &treeNode = m_allocator[nodeIndex];

        if(treeNode.preTraverse(this))
        {
          doTraverseDown(treeNode.getFirstChild());
        }

        nodeIndex = treeNode.getNextSibling();
        treeNode.postTraverse(this);

        if(m_stopTraversal)
        {
          return;
        }
      }
    }

    void Traverser::postAscendTraverse()
    {
    }

    bool Traverser::ascendTraverse(TreeNode& treeNode)
    {
      return true;
    }

    bool Traverser::preTraverse(TreeNode& treeNode)
    {
      return true;
    }

    void Traverser::postTraverse(TreeNode& treeNode)
    {
    }

    bool Traverser::ascendTraverse(GroupNode& treeNode)
    {
      return true;
    }

    bool Traverser::preTraverse(GroupNode& treeNode)
    {
      return true;
    }

    void Traverser::postTraverse(GroupNode& treeNode)
    {
    }

    bool Traverser::ascendTraverse(AnimatedTransformNode& treeNode)
    {
      return true;
    }

    bool Traverser::preTraverse(AnimatedTransformNode& treeNode)
    {
      return true;
    }

    void Traverser::postTraverse(AnimatedTransformNode& treeNode)
    {
    }

    bool Traverser::ascendTraverse(TransformNode& treeNode)
    {
      return true;
    }

    bool Traverser::preTraverse(TransformNode& treeNode)
    {
      return true;
    }

    void Traverser::postTraverse(TransformNode& treeNode)
    {
    }

    bool Traverser::ascendTraverse(LODNode& treeNode)
    {
      return true;
    }

    bool Traverser::preTraverse(LODNode& treeNode)
    {
      return true;
    }

    void Traverser::postTraverse(LODNode& treeNode)
    {
    }

    bool Traverser::ascendTraverse(AnimatedGeoNode& treeNode)
    {
      return true;
    }

    bool Traverser::preTraverse(AnimatedGeoNode& treeNode)
    {
      return true;
    }

    void Traverser::postTraverse(AnimatedGeoNode& treeNode)
    {
    }

    bool Traverser::ascendTraverse(GeoNode& treeNode)
    {
      return true;
    }

    bool Traverser::preTraverse(GeoNode& treeNode)
    {
      return true;
    }

    void Traverser::postTraverse(GeoNode& treeNode)
    {
    }

    bool Traverser::ascendTraverse(BillboardNode& treeNode)
    {
      return true;
    }

    bool Traverser::preTraverse(BillboardNode& treeNode)
    {
      return true;
    }

    void Traverser::postTraverse(BillboardNode& treeNode)
    {
    }

    bool Traverser::ascendTraverse(ParticleNode& treeNode)
    {
      return true;
    }

    bool Traverser::preTraverse(ParticleNode& treeNode)
    {
      return true;
    }

    void Traverser::postTraverse(ParticleNode& treeNode)
    {
    }

    bool Traverser::ascendTraverse(LightNode& treeNode)
    {
      return true;
    }

    bool Traverser::preTraverse(LightNode& treeNode)
    {
      return true;
    }

    void Traverser::postTraverse(LightNode& treeNode)
    {
    }

    bool Traverser::ascendTraverse(ShadowLightNode& treeNode)
    {
      return true;
    }

    bool Traverser::preTraverse(ShadowLightNode& treeNode)
    {
      return true;
    }

    void Traverser::postTraverse(ShadowLightNode& treeNode)
    {
    }
  }
}
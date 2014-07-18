#include "SceneGraph/Traverser/ConstTraverser.h"

#include "SceneGraph/Scene/Scene.h"

#include "SceneGraph/TreeNodes/TreeNode.h"
#include "SceneGraph/TreeNodes/GroupNode.h"

namespace he
{
  namespace sg
  {
    ConstTraverser::ConstTraverser(const TreeNodeAllocator& allocator) : m_allocator(allocator), m_stopTraversal(false)
    {
    }

    ConstTraverser::~ConstTraverser()
    {
    }

    void ConstTraverser::doAscend(const TreeNode &treeNode)
    {
      NodeIndex index = treeNode.getParent();
      while(index != ~0)
      {
        const TreeNode& treeNode2 = m_allocator[index];

        if(!treeNode2.ascendTraverse(this) || m_stopTraversal)
        {
          return;
        }

        index = treeNode2.getParent();
      }

      postAscendTraverse();
    }

    void ConstTraverser::doTraverse(const TreeNode &treeNode)
    {
      if(treeNode.preTraverse(this))
      {
        doTraverseDown(treeNode.getFirstChild());
      }
      treeNode.postTraverse(this);
    }

    void ConstTraverser::doTraverseDown(NodeIndex nodeIndex)
    {
      while(nodeIndex != ~0)
      {
        const TreeNode& treeNode = m_allocator[nodeIndex];

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

    void ConstTraverser::postAscendTraverse()
    {
    }

    bool ConstTraverser::ascendTraverse(const TreeNode& treeNode)
    {
      return true;
    }

    bool ConstTraverser::preTraverse(const TreeNode& treeNode)
    {
      return true;
    }

    void ConstTraverser::postTraverse(const TreeNode& treeNode)
    {
    }

    bool ConstTraverser::ascendTraverse(const GroupNode& treeNode)
    {
      return true;
    }

    bool ConstTraverser::preTraverse(const GroupNode& treeNode)
    {
      return true;
    }

    void ConstTraverser::postTraverse(const GroupNode& treeNode)
    {
    }

    bool ConstTraverser::ascendTraverse(const AnimatedTransformNode& treeNode)
    {
      return true;
    }

    bool ConstTraverser::preTraverse(const AnimatedTransformNode& treeNode)
    {
      return true;
    }

    void ConstTraverser::postTraverse(const AnimatedTransformNode& treeNode)
    {
    }

    bool ConstTraverser::ascendTraverse(const TransformNode& treeNode)
    {
      return true;
    }

    bool ConstTraverser::preTraverse(const TransformNode& treeNode)
    {
      return true;
    }

    void ConstTraverser::postTraverse(const TransformNode& treeNode)
    {
    }

    bool ConstTraverser::ascendTraverse(const LODNode& treeNode)
    {
      return true;
    }

    bool ConstTraverser::preTraverse(const LODNode& treeNode)
    {
      return true;
    }

    void ConstTraverser::postTraverse(const LODNode& treeNode)
    {
    }

    bool ConstTraverser::ascendTraverse(const AnimatedGeoNode& treeNode)
    {
      return true;
    }

    bool ConstTraverser::preTraverse(const AnimatedGeoNode& treeNode)
    {
      return true;
    }

    void ConstTraverser::postTraverse(const AnimatedGeoNode& treeNode)
    {
    }

    bool ConstTraverser::ascendTraverse(const GeoNode& treeNode)
    {
      return true;
    }

    bool ConstTraverser::preTraverse(const GeoNode& treeNode)
    {
      return true;
    }

    void ConstTraverser::postTraverse(const GeoNode& treeNode)
    {
    }

    bool ConstTraverser::ascendTraverse(const BillboardNode& treeNode)
    {
      return true;
    }

    bool ConstTraverser::preTraverse(const BillboardNode& treeNode)
    {
      return true;
    }

    void ConstTraverser::postTraverse(const BillboardNode& treeNode)
    {
    }

    bool ConstTraverser::ascendTraverse(const ParticleNode& treeNode)
    {
      return true;
    }

    bool ConstTraverser::preTraverse(const ParticleNode& treeNode)
    {
      return true;
    }

    void ConstTraverser::postTraverse(const ParticleNode& treeNode)
    {
    }

    bool ConstTraverser::ascendTraverse(const LightNode& treeNode)
    {
      return true;
    }

    bool ConstTraverser::preTraverse(const LightNode& treeNode)
    {
      return true;
    }

    void ConstTraverser::postTraverse(const LightNode& treeNode)
    {
    }

    bool ConstTraverser::ascendTraverse(const ShadowLightNode& treeNode)
    {
      return true;
    }

    bool ConstTraverser::preTraverse(const ShadowLightNode& treeNode)
    {
      return true;
    }

    void ConstTraverser::postTraverse(const ShadowLightNode& treeNode)
    {
    }
  }
}
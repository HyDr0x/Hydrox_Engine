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
      if(treeNode.getParent() != ~0)
      {
        NodeIndex parentNodeIndex = treeNode.getParent();

        while(m_allocator[parentNodeIndex].getParent() != ~0)//start calculating the trfMatrix of the upper path of the actual node
        {
          if(!m_allocator[parentNodeIndex].ascendTraverse(this) || m_stopTraversal)
          {
            return;
          }

          parentNodeIndex = m_allocator[parentNodeIndex].getParent();
        }
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
        if(m_allocator[nodeIndex].preTraverse(this))
        {
          doTraverseDown(m_allocator[nodeIndex].getFirstChild());
        }

        NodeIndex nodeIndexTmp = m_allocator[nodeIndex].getNextSibling();
        m_allocator[nodeIndex].postTraverse(this);
        nodeIndex = nodeIndexTmp;

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
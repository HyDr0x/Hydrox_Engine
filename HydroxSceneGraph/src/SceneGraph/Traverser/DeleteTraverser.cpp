#include "SceneGraph/Traverser/DeleteTraverser.h"

#include "SceneGraph/TreeNodes/BillboardNode.h"
#include "SceneGraph/TreeNodes/AnimatedTransformNode.h"
#include "SceneGraph/TreeNodes/AnimatedGeoNode.h"
#include "SceneGraph/TreeNodes/GeoNode.h"
#include "SceneGraph/TreeNodes/LightNode.h"
#include "SceneGraph/TreeNodes/ShadowLightNode.h"
#include "SceneGraph/TreeNodes/LODNode.h"
#include "SceneGraph/TreeNodes/ParticleNode.h"
#include "SceneGraph/TreeNodes/TransformNode.h"

#include "SceneGraph/Scene/TreeNodeAllocator.h"

namespace he
{
  namespace sg
  {
    DeleteTraverser::DeleteTraverser(TreeNodeAllocator& allocator) : Traverser(allocator)
    {
    }

    DeleteTraverser::~DeleteTraverser()
    {
    }

    void DeleteTraverser::postTraverse(AnimatedTransformNode& treeNode)
    {
      m_allocator.erase(treeNode.getNodeIndex());
    }

    void DeleteTraverser::postTraverse(TransformNode& treeNode)
    {
      m_allocator.erase(treeNode.getNodeIndex());
    }

    void DeleteTraverser::postTraverse(LODNode& treeNode)
    {
      m_allocator.erase(treeNode.getNodeIndex());
    }

    void DeleteTraverser::postTraverse(AnimatedGeoNode& treeNode)
    {
      m_allocator.erase(treeNode.getNodeIndex());
    }

    void DeleteTraverser::postTraverse(GeoNode& treeNode)
    {
      m_allocator.erase(treeNode.getNodeIndex());
    }

    void DeleteTraverser::postTraverse(BillboardNode& treeNode)
    {
      m_allocator.erase(treeNode.getNodeIndex());
    }

    void DeleteTraverser::postTraverse(ParticleNode& treeNode)
    {
      m_allocator.erase(treeNode.getNodeIndex());
    }

    void DeleteTraverser::postTraverse(LightNode& treeNode)
    {
      m_allocator.erase(treeNode.getNodeIndex());
    }

    void DeleteTraverser::postTraverse(ShadowLightNode& treeNode)
    {
      m_allocator.erase(treeNode.getNodeIndex());
    }
  }
}
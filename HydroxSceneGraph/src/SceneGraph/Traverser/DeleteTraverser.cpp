#include "SceneGraph/Traverser/DeleteTraverser.h"

#include "SceneGraph/TreeNodes/BillboardNode.h"
#include "SceneGraph/TreeNodes/AnimatedTransformNode.h"
#include "SceneGraph/TreeNodes/AnimatedGeoNode.h"
#include "SceneGraph/TreeNodes/GeoNode.h"
#include "SceneGraph/TreeNodes/LightNode.h"
#include "SceneGraph/TreeNodes/LODNode.h"
#include "SceneGraph/TreeNodes/ParticleNode.h"
#include "SceneGraph/TreeNodes/TransformNode.h"

namespace he
{
  namespace sg
  {
    DeleteTraverser::DeleteTraverser()
    {
    }

    DeleteTraverser::~DeleteTraverser()
    {
    }

    bool DeleteTraverser::preTraverse(const AnimatedTransformNode* treeNode)
    {
      return true;
    }

    void DeleteTraverser::postTraverse(const AnimatedTransformNode* treeNode)
    {
      delete treeNode;
      treeNode = nullptr;
    }

    bool DeleteTraverser::preTraverse(const TransformNode* treeNode)
    {
      return true;
    }

    void DeleteTraverser::postTraverse(const TransformNode* treeNode)
    {
      delete treeNode;
      treeNode = nullptr;
    }

    bool DeleteTraverser::preTraverse(const LODNode* treeNode)
    {
      return true;
    }

    void DeleteTraverser::postTraverse(const LODNode* treeNode)
    {
      delete treeNode;
      treeNode = nullptr;
    }

    bool DeleteTraverser::preTraverse(const AnimatedGeoNode* treeNode)
    {
      return true;
    }

    void DeleteTraverser::postTraverse(const AnimatedGeoNode* treeNode)
    {
      delete treeNode;
      treeNode = nullptr;
    }

    bool DeleteTraverser::preTraverse(const GeoNode* treeNode)
    {
      return true;
    }

    void DeleteTraverser::postTraverse(const GeoNode* treeNode)
    {
      delete treeNode;
      treeNode = nullptr;
    }

    bool DeleteTraverser::preTraverse(const BillboardNode* treeNode)
    {
      return true;
    }

    void DeleteTraverser::postTraverse(const BillboardNode* treeNode)
    {
      delete treeNode;
      treeNode = nullptr;
    }

    bool DeleteTraverser::preTraverse(const ParticleNode* treeNode)
    {
      return true;
    }

    void DeleteTraverser::postTraverse(const ParticleNode* treeNode)
    {
      delete treeNode;
      treeNode = nullptr;
    }

    bool DeleteTraverser::preTraverse(const LightNode* treeNode)
    {
      return true;
    }

    void DeleteTraverser::postTraverse(const LightNode* treeNode)
    {
      delete treeNode;
      treeNode = nullptr;
    }
  }
}
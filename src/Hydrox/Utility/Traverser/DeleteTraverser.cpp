#include "Hydrox/Utility/Traverser/DeleteTraverser.h"

#include "Hydrox/Utility/Tree/BillboardNode.h"
#include "Hydrox/Utility/Tree/AnimatedTransformNode.h"
#include "Hydrox/Utility/Tree/AnimatedGeoNode.h"
#include "Hydrox/Utility/Tree/GeoNode.h"
#include "Hydrox/Utility/Tree/LightNode.h"
#include "Hydrox/Utility/Tree/LODNode.h"
#include "Hydrox/Utility/Tree/ParticleNode.h"
#include "Hydrox/Utility/Tree/TransformNode.h"

namespace he
{
  DeleteTraverser::DeleteTraverser()
  {
    m_postKill = true;
    m_lastVisitedNode = nullptr;
  }

  DeleteTraverser::~DeleteTraverser()
  {
  }

  bool DeleteTraverser::preTraverse(AnimatedTransformNode* treeNode)
  {
    return true;
  }

  void DeleteTraverser::postTraverse(AnimatedTransformNode* treeNode)
  {
    delete treeNode;
    treeNode = nullptr;
  }

  bool DeleteTraverser::preTraverse(TransformNode* treeNode)
  {
    return true;
  }

  void DeleteTraverser::postTraverse(TransformNode* treeNode)
  {
    delete treeNode;
    treeNode = nullptr;
  }

  bool DeleteTraverser::preTraverse(LODNode* treeNode)
  {
    return true;
  }

  void DeleteTraverser::postTraverse(LODNode* treeNode)
  {
    delete treeNode;
    treeNode = nullptr;
  }

  bool DeleteTraverser::preTraverse(AnimatedGeoNode* treeNode)
  {
    return true;
  }

  void DeleteTraverser::postTraverse(AnimatedGeoNode* treeNode)
  {
    delete treeNode;
    treeNode = nullptr;
  }

  bool DeleteTraverser::preTraverse(GeoNode* treeNode)
  {
    return true;
  }

  void DeleteTraverser::postTraverse(GeoNode* treeNode)
  {
    delete treeNode;
    treeNode = nullptr;
  }

  bool DeleteTraverser::preTraverse(BillboardNode* treeNode)
  {
    return true;
  }

  void DeleteTraverser::postTraverse(BillboardNode* treeNode)
  {
    delete treeNode;
    treeNode = nullptr;
  }

  bool DeleteTraverser::preTraverse(ParticleNode* treeNode)
  {
    return true;
  }

  void DeleteTraverser::postTraverse(ParticleNode* treeNode)
  {
    delete treeNode;
    treeNode = nullptr;
  }

  bool DeleteTraverser::preTraverse(LightNode* treeNode)
  {
    return true;
  }

  void DeleteTraverser::postTraverse(LightNode* treeNode)
  {
    delete treeNode;
    treeNode = nullptr;
  }
}
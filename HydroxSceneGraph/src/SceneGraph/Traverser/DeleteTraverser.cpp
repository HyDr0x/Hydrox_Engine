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
}
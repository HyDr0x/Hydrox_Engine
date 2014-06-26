#include "Saver/NodeLinkTraverser.h"

#include <sstream>

#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <DataBase/Mesh.h>
#include <DataBase/Material.h>
#include <DataBase/Texture2D.h>
#include <DataBase/RenderShader.h>

#include <SceneGraph/TreeNodes/GeoNode.h>
#include <SceneGraph/TreeNodes/AnimatedGeoNode.h>
#include <SceneGraph/TreeNodes/TransformNode.h>
#include <SceneGraph/TreeNodes/AnimatedTransformNode.h>
#include <SceneGraph/TreeNodes/LODNode.h>
#include <SceneGraph/TreeNodes/BillboardNode.h>
#include <SceneGraph/TreeNodes/LightNode.h>
#include <SceneGraph/TreeNodes/ShadowLightNode.h>
#include <SceneGraph/TreeNodes/ParticleNode.h>

#include "Saver/NodeWrapper.h"
#include "Saver/NodeWrapperMapper.h"

namespace he
{
  namespace saver
  {
    NodeLinkTraverser::NodeLinkTraverser(NodeWrapperMapper& wrapperMapper) : 
      m_wrapperMapper(wrapperMapper)
    {
    }

    NodeLinkTraverser::~NodeLinkTraverser()
    {
    }

    bool NodeLinkTraverser::preTraverse(sg::TransformNode* treeNode)
    {
      TransformNodeData& data = m_wrapperMapper.transformNodes[m_wrapperMapper.transformNodeMap[treeNode]];
      findNodeIndex(treeNode->getFirstChild(), data.firstChildIndex, data.firstChildNodeType);
      findNodeIndex(treeNode->getNextSibling(), data.nextSiblingIndex, data.nextSiblingNodeType);
      findNodeIndex(treeNode->getParent(), data.parentIndex, data.parentNodeType);

      return true;
    }

    bool NodeLinkTraverser::preTraverse(sg::AnimatedTransformNode* treeNode)
    {
      AnimatedTransformNodeData& data = m_wrapperMapper.animatedTransformNodes[m_wrapperMapper.animatedTransformNodeMap[treeNode]];

      if(treeNode->getSkinnedMesh() != nullptr)
      {
        data.animatedMeshIndex = m_wrapperMapper.animatedGeoNodeMap[treeNode->getSkinnedMesh()];
      }
      else
      {
        data.animatedMeshIndex = ~0;
      }

      findNodeIndex(treeNode->getFirstChild(), data.firstChildIndex, data.firstChildNodeType);
      findNodeIndex(treeNode->getNextSibling(), data.nextSiblingIndex, data.nextSiblingNodeType);
      findNodeIndex(treeNode->getParent(), data.parentIndex, data.parentNodeType);

      return true;
    }

    bool NodeLinkTraverser::preTraverse(sg::GeoNode* treeNode)
    {
      GeoNodeData& data = m_wrapperMapper.geoNodes[m_wrapperMapper.geoNodeMap[treeNode]];
      findNodeIndex(treeNode->getFirstChild(), data.firstChildIndex, data.firstChildNodeType);
      findNodeIndex(treeNode->getNextSibling(), data.nextSiblingIndex, data.nextSiblingNodeType);
      findNodeIndex(treeNode->getParent(), data.parentIndex, data.parentNodeType);

      return true;
    }

    bool NodeLinkTraverser::preTraverse(sg::AnimatedGeoNode* treeNode)
    {
      AnimatedGeoNodeData& data = m_wrapperMapper.animatedGeoNodes[m_wrapperMapper.animatedGeoNodeMap[treeNode]];
      findNodeIndex(treeNode->getFirstChild(), data.firstChildIndex, data.firstChildNodeType);
      findNodeIndex(treeNode->getNextSibling(), data.nextSiblingIndex, data.nextSiblingNodeType);
      findNodeIndex(treeNode->getParent(), data.parentIndex, data.parentNodeType);

      return true;
    }

    bool NodeLinkTraverser::preTraverse(sg::BillboardNode* treeNode)
    {
      BillboardNodeData& data = m_wrapperMapper.billboardNodes[m_wrapperMapper.billboardNodeMap[treeNode]];
      findNodeIndex(treeNode->getFirstChild(), data.firstChildIndex, data.firstChildNodeType);
      findNodeIndex(treeNode->getNextSibling(), data.nextSiblingIndex, data.nextSiblingNodeType);
      findNodeIndex(treeNode->getParent(), data.parentIndex, data.parentNodeType);

      return true;
    }

    bool NodeLinkTraverser::preTraverse(sg::LODNode* treeNode)
    {
      LODNodeData& data = m_wrapperMapper.lodNodes[m_wrapperMapper.lodNodeMap[treeNode]];
      findNodeIndex(treeNode->getFirstChild(), data.firstChildIndex, data.firstChildNodeType);
      findNodeIndex(treeNode->getNextSibling(), data.nextSiblingIndex, data.nextSiblingNodeType);
      findNodeIndex(treeNode->getParent(), data.parentIndex, data.parentNodeType);

      return true;
    }

    bool NodeLinkTraverser::preTraverse(sg::LightNode* treeNode)
    {
      LightNodeData& data = m_wrapperMapper.lightNodes[m_wrapperMapper.lightNodeMap[treeNode]];
      findNodeIndex(treeNode->getFirstChild(), data.firstChildIndex, data.firstChildNodeType);
      findNodeIndex(treeNode->getNextSibling(), data.nextSiblingIndex, data.nextSiblingNodeType);
      findNodeIndex(treeNode->getParent(), data.parentIndex, data.parentNodeType);

      return true;
    }

    bool NodeLinkTraverser::preTraverse(sg::ShadowLightNode* treeNode)
    {
      LightNodeData& data = m_wrapperMapper.shadowLightNodes[m_wrapperMapper.shadowLightNodeMap[treeNode]];
      findNodeIndex(treeNode->getFirstChild(), data.firstChildIndex, data.firstChildNodeType);
      findNodeIndex(treeNode->getNextSibling(), data.nextSiblingIndex, data.nextSiblingNodeType);
      findNodeIndex(treeNode->getParent(), data.parentIndex, data.parentNodeType);

      return true;
    }

    bool NodeLinkTraverser::preTraverse(sg::ParticleNode* treeNode)
    {
      ParticleNodeData& data = m_wrapperMapper.particleNodes[m_wrapperMapper.particleNodeMap[treeNode]];
      findNodeIndex(treeNode->getFirstChild(), data.firstChildIndex, data.firstChildNodeType);
      findNodeIndex(treeNode->getNextSibling(), data.nextSiblingIndex, data.nextSiblingNodeType);
      findNodeIndex(treeNode->getParent(), data.parentIndex, data.parentNodeType);

      return true;
    }

    void NodeLinkTraverser::findNodeIndex(sg::TreeNode *treeNode, unsigned int& index, NodeType& nodeType)
    {
      if(treeNode != nullptr)
      {
        if(treeNode->isAnimatedGeoNode())
        {
          index = m_wrapperMapper.animatedGeoNodeMap[dynamic_cast<sg::AnimatedGeoNode*>(treeNode)];
          nodeType = ANIMATEDGEONODE;
        }
        else if(treeNode->isGeoNode())
        {
          index = m_wrapperMapper.geoNodeMap[dynamic_cast<sg::GeoNode*>(treeNode)];
          nodeType = GEONODE;
        }
        else if(treeNode->isAnimatedTransformNode())
        {
          index = m_wrapperMapper.animatedTransformNodeMap[dynamic_cast<sg::AnimatedTransformNode*>(treeNode)];
          nodeType = ANIMATEDTRANSFORMNODE;
        }
        else if(treeNode->isTransformNode())
        {
          index = m_wrapperMapper.transformNodeMap[dynamic_cast<sg::TransformNode*>(treeNode)];
          nodeType = TRANSFORMNODE;
        }
        else if(treeNode->isBillboardNode())
        {
          index = m_wrapperMapper.billboardNodeMap[dynamic_cast<sg::BillboardNode*>(treeNode)];
          nodeType = BILLBOARDNODE;
        }
        else if(treeNode->isLODNode())
        {
          index = m_wrapperMapper.lodNodeMap[dynamic_cast<sg::LODNode*>(treeNode)];
          nodeType = LODNODE;
        }
        else if(treeNode->isShadowedLightNode())
        {
          index = m_wrapperMapper.shadowLightNodeMap[dynamic_cast<sg::ShadowLightNode*>(treeNode)];
          nodeType = SHADOWLIGHTNODE;
        }
        else if(treeNode->isLightNode())
        {
          index = m_wrapperMapper.lightNodeMap[dynamic_cast<sg::LightNode*>(treeNode)];
          nodeType = LIGHTNODE;
        }
        else if(treeNode->isParticleNode())
        {
          index = m_wrapperMapper.particleNodeMap[dynamic_cast<sg::ParticleNode*>(treeNode)];
          nodeType = PARTICLENODE;
        }
      }
    }
  }
}

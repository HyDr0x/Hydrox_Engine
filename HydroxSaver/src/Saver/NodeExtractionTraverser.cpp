#include "Saver/NodeExtractionTraverser.h"

#include <sstream>

#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <DataBase/Mesh.h>
#include <DataBase/Material.h>
#include <DataBase/Texture2D.h>
#include <DataBase/RenderShader.h>

#include <SceneGraph/Scene/TreeNodeAllocator.h>

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
    NodeExtractionTraverser::NodeExtractionTraverser(const sg::TreeNodeAllocator& allocator, std::string fileName, std::map<std::string, std::map<util::ResourceHandle, std::string, util::Less>>& resourceMap) :
      ConstTraverser(allocator), m_resourceMap(resourceMap), m_fileName(fileName)
    {
    }

    NodeExtractionTraverser::~NodeExtractionTraverser()
    {
    }

    bool NodeExtractionTraverser::preTraverse(sg::GeoNode& treeNode)
    {
      if(!m_resourceMap["Meshes"].count(treeNode.getMeshHandle()))
      {
        m_resourceMap["Meshes"][treeNode.getMeshHandle()] = m_fileName + treeNode.getNodeName() + "_Mesh";
      }

      if(!m_resourceMap["Materials"].count(treeNode.getMaterialHandle()))
      {
        m_resourceMap["Materials"][treeNode.getMaterialHandle()] = m_fileName + treeNode.getNodeName() + "_Material";
      }

      return true;
    }

    bool NodeExtractionTraverser::preTraverse(sg::AnimatedGeoNode& treeNode)
    {
      if(!m_resourceMap["Meshes"].count(treeNode.getMeshHandle()))
      {
        m_resourceMap["Meshes"][treeNode.getMeshHandle()] = m_fileName + treeNode.getNodeName() + "_Mesh";
      }

      if(!m_resourceMap["Materials"].count(treeNode.getMaterialHandle()))
      {
        m_resourceMap["Materials"][treeNode.getMaterialHandle()] = m_fileName + treeNode.getNodeName() + "_Material";
      }

      return true;
    }

    bool NodeExtractionTraverser::preTraverse(sg::BillboardNode& treeNode)
    {
      if(!m_resourceMap["Textures"].count(treeNode.getTextureHandle()))
      {
        m_resourceMap["Textures"][treeNode.getTextureHandle()] = m_fileName + treeNode.getNodeName() + "_Texture";
      }

      return true;
    }
  }
}

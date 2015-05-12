#include "Saver/NodeExtractionTraverser.h"

#include <sstream>

#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <DataBase/Mesh.h>
#include <DataBase/Material.h>
#include <DataBase/Texture2D.h>

#include <Shader/ShaderContainer.h>

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

namespace he
{
  namespace saver
  {
    NodeExtractionTraverser::NodeExtractionTraverser(const sg::TreeNodeAllocator& allocator, std::string fileName,
      std::map<std::string, std::map<util::ResourceHandle, std::string, util::Less>>& resourceMap) :
      ConstTraverser(allocator), m_fileName(fileName), m_resourceMap(resourceMap)
    {
    }

    NodeExtractionTraverser::~NodeExtractionTraverser()
    {
    }

    bool NodeExtractionTraverser::preTraverse(const sg::GeoNode& treeNode)
    {
      if(!m_resourceMap["Meshes"].count(treeNode.getMeshHandle()))
      {
        std::stringstream stream;
        stream << m_resourceMap["Meshes"].size();
        m_resourceMap["Meshes"][treeNode.getMeshHandle()] = m_fileName + std::string("_Mesh_") + stream.str();
      }

      if(!m_resourceMap["Materials"].count(treeNode.getMaterialHandle()))
      {
        std::stringstream stream;
        stream << m_resourceMap["Materials"].size();
        m_resourceMap["Materials"][treeNode.getMaterialHandle()] = m_fileName + "_Material_" + stream.str();
      }

      return true;
    }

    bool NodeExtractionTraverser::preTraverse(const sg::AnimatedGeoNode& treeNode)
    {
      return preTraverse((sg::GeoNode&) treeNode);
    }

    bool NodeExtractionTraverser::preTraverse(const sg::BillboardNode& treeNode)
    {
      if(!m_resourceMap["Textures"].count(treeNode.getTextureHandle()))
      {
        std::stringstream stream;
        stream << m_resourceMap["Textures"].size();
        m_resourceMap["Textures"][treeNode.getTextureHandle()] = m_fileName + "_Texture_" + stream.str();
      }

      return true;
    }
  }
}

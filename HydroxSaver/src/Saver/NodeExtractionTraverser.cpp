#include "Saver/NodeExtractionTraverser.h"

#include <sstream>

#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <Renderer/Resources/Mesh.h>
#include <Renderer/Resources/Material.h>
#include <Renderer/Resources/Texture2D.h>
#include <Renderer/Resources/RenderShader.h>

#include <SceneGraph/TreeNodes/GeoNode.h>
#include <SceneGraph/TreeNodes/AnimatedGeoNode.h>
#include <SceneGraph/TreeNodes/TransformNode.h>
#include <SceneGraph/TreeNodes/AnimatedTransformNode.h>
#include <SceneGraph/TreeNodes/LODNode.h>
#include <SceneGraph/TreeNodes/BillboardNode.h>
#include <SceneGraph/TreeNodes/LightNode.h>
#include <SceneGraph/TreeNodes/ParticleNode.h>

#include "Saver/NodeWrapper.h"
#include "Saver/NodeWrapperMapper.h"

namespace he
{
  namespace saver
  {    NodeExtractionTraverser::NodeExtractionTraverser(std::string fileName, NodeWrapperMapper& wrapperMapper, util::SingletonManager *singletonManager) : m_wrapperMapper(wrapperMapper), m_fileName(fileName)
    {
      m_modelManager = singletonManager->getService<renderer::ModelManager>();
      m_materialManager = singletonManager->getService<renderer::MaterialManager>();
      m_renderShaderManager = singletonManager->getService<renderer::RenderShaderManager>();
      m_textureManager = singletonManager->getService<renderer::TextureManager>();
    }

    NodeExtractionTraverser::~NodeExtractionTraverser()
    {
    }

    bool NodeExtractionTraverser::preTraverse(sg::TransformNode* treeNode)
    {
      if(!m_wrapperMapper.transformNodeMap.count(treeNode))
      {
        TransformNodeData data;
        data.nodeName = treeNode->getNodeName();
        data.translation = treeNode->getLocalPosition();
        data.rotation = treeNode->getLocalRotation();
        data.scale = treeNode->getLocalScale();

        m_wrapperMapper.transformNodeMap[treeNode] = m_wrapperMapper.transformNodes.size();
        m_wrapperMapper.transformNodes.push_back(data);
      }

      return true;
    }

    bool NodeExtractionTraverser::preTraverse(sg::AnimatedTransformNode* treeNode)
    {
      if(!m_wrapperMapper.animatedTransformNodeMap.count(treeNode))
      {
        AnimatedTransformNodeData data;
        data.nodeName = treeNode->getNodeName();
        data.translation = treeNode->getLocalPosition();
        data.rotation = treeNode->getLocalRotation();
        data.scale = treeNode->getLocalScale();

        data.boneIndex = treeNode->getBoneIndex();
        data.animationTracks = treeNode->getAnimationTracks();

        m_wrapperMapper.animatedTransformNodeMap[treeNode] = m_wrapperMapper.animatedTransformNodes.size();
        m_wrapperMapper.animatedTransformNodes.push_back(data);
      }

      return true;
    }

    bool NodeExtractionTraverser::preTraverse(sg::GeoNode* treeNode)
    {
      if(!m_wrapperMapper.geoNodeMap.count(treeNode))
      {
        GeoNodeData data;

        data.nodeName = treeNode->getNodeName();

        
        if(!m_wrapperMapper.meshMap.count(treeNode->getMeshHandle()))
        {
          m_wrapperMapper.meshMap[treeNode->getMeshHandle()] = m_wrapperMapper.meshes.size();
          data.meshIndex = m_wrapperMapper.meshes.size();

          renderer::Mesh *mesh = m_modelManager->getObject(treeNode->getMeshHandle());
          m_wrapperMapper.meshes.push_back(*mesh);
        }
        else
        {
          data.meshIndex = m_wrapperMapper.meshMap[treeNode->getMeshHandle()];
        }

        if(!m_wrapperMapper.materialMap.count(treeNode->getMaterialHandle()))
        {
          m_wrapperMapper.materialMap[treeNode->getMaterialHandle()] = m_wrapperMapper.materialFileNames.size();
          data.materialIndex = m_wrapperMapper.materialFileNames.size();

          renderer::Material *material = m_materialManager->getObject(treeNode->getMaterialHandle());

          std::stringstream materialFileName;
          materialFileName << m_fileName << "_Material_" << m_wrapperMapper.materialFileNames.size();

          m_wrapperMapper.materialFileNames.push_back(materialFileName.str());
        }
        else
        {
          data.materialIndex = m_wrapperMapper.materialMap[treeNode->getMaterialHandle()];
        }

        
        m_wrapperMapper.geoNodeMap[treeNode] = m_wrapperMapper.geoNodes.size();
        m_wrapperMapper.geoNodes.push_back(data);
      }

      return true;
    }

    bool NodeExtractionTraverser::preTraverse(sg::AnimatedGeoNode* treeNode)
    {
      if(!m_wrapperMapper.animatedGeoNodeMap.count(treeNode))
      {
        AnimatedGeoNodeData data;
        data.nodeName = treeNode->getNodeName();
        data.inverseBindPoseMatrices = treeNode->getInverseBindPoseMatrices();

        if(!m_wrapperMapper.meshMap.count(treeNode->getMeshHandle()))
        {
          m_wrapperMapper.meshMap[treeNode->getMeshHandle()] = m_wrapperMapper.meshes.size();
          data.meshIndex = m_wrapperMapper.meshes.size();

          renderer::Mesh *mesh = m_modelManager->getObject(treeNode->getMeshHandle());
          m_wrapperMapper.meshes.push_back(*mesh);
        }
        else
        {
          data.meshIndex = m_wrapperMapper.meshMap[treeNode->getMeshHandle()];
        }

        if(!m_wrapperMapper.materialMap.count(treeNode->getMaterialHandle()))
        {
          m_wrapperMapper.materialMap[treeNode->getMaterialHandle()] = m_wrapperMapper.materialFileNames.size();
          data.materialIndex = m_wrapperMapper.materialFileNames.size();

          renderer::Material *material = m_materialManager->getObject(treeNode->getMaterialHandle());

          std::stringstream materialFileName;
          materialFileName << m_fileName << "_Material_" << m_wrapperMapper.materialFileNames.size();

          m_wrapperMapper.materialFileNames.push_back(materialFileName.str());
        }
        else
        {
          data.materialIndex = m_wrapperMapper.materialMap[treeNode->getMaterialHandle()];
        }

        m_wrapperMapper.animatedGeoNodeMap[treeNode] = m_wrapperMapper.animatedGeoNodes.size();
        m_wrapperMapper.animatedGeoNodes.push_back(data);
      }

      return true;
    }

    bool NodeExtractionTraverser::preTraverse(sg::BillboardNode* treeNode)
    {
      if(!m_wrapperMapper.billboardNodeMap.count(treeNode))
      {
        BillboardNodeData data;
        data.nodeName = treeNode->getNodeName();
        data.animCount = treeNode->getAnimationCount();
        data.animNumber = treeNode->getAnimationNumber();
        data.translate = treeNode->getPosition();
        data.scale = treeNode->getScale();
        data.texStart = treeNode->getTextureStart();
        data.texEnd = treeNode->getTextureEnd();

        if(!m_wrapperMapper.billboardTextureMap.count(treeNode->getTextureHandle()))
        {
          //renderer::Texture2D *texture = m_textureManager->getObject(treeNode->getTextureHandle());

          std::stringstream billboardTextureFileName;
          billboardTextureFileName << m_fileName << "_BillboardTexture_" << m_wrapperMapper.billboardTextureFileNames.size();

          m_wrapperMapper.billboardTextureMap[treeNode->getTextureHandle()] = m_wrapperMapper.billboardTextureFileNames.size();

          data.textureIndex = m_wrapperMapper.billboardTextureFileNames.size();

          m_wrapperMapper.billboardTextureFileNames.push_back(billboardTextureFileName.str());
        }
        else
        {
          data.textureIndex = m_wrapperMapper.billboardTextureMap[treeNode->getTextureHandle()];
        }

        m_wrapperMapper.billboardNodeMap[treeNode] = m_wrapperMapper.billboardNodes.size();
        m_wrapperMapper.billboardNodes.push_back(data);
      }

      return true;
    }

    bool NodeExtractionTraverser::preTraverse(sg::LODNode* treeNode)
    {
      if(!m_wrapperMapper.lodNodeMap.count(treeNode))
      {
        LODNodeData data;
        data.nodeName = treeNode->getNodeName();
        data.lodLevel = treeNode->getLODLevel();
        data.position = treeNode->getPosition();

        m_wrapperMapper.lodNodeMap[treeNode] = m_wrapperMapper.lodNodes.size();
        m_wrapperMapper.lodNodes.push_back(data);
      }

      return true;
    }

    bool NodeExtractionTraverser::preTraverse(sg::LightNode* treeNode)
    {
      if(!m_wrapperMapper.lightNodeMap.count(treeNode))
      {
        LightNodeData data;
        data.nodeName = treeNode->getNodeName();
        data.trfMatrix = treeNode->getTransformationMatrix();

        m_wrapperMapper.lightNodeMap[treeNode] = m_wrapperMapper.lightNodes.size();
        m_wrapperMapper.lightNodes.push_back(data);
      }

      return true;
    }

    bool NodeExtractionTraverser::preTraverse(sg::ParticleNode* treeNode)
    {
      if(!m_wrapperMapper.particleNodeMap.count(treeNode))
      {
        ParticleNodeData data;
        data.nodeName = treeNode->getNodeName();
        data.trfMatrix = treeNode->getTransformationMatrix();

        m_wrapperMapper.particleNodeMap[treeNode] = m_wrapperMapper.particleNodes.size();
        m_wrapperMapper.particleNodes.push_back(data);
      }

      return true;
    }
  }
}

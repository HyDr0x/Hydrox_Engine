#include "Saver/NodeExtractionTraverser.h"

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
    NodeExtractionTraverser::NodeExtractionTraverser(std::string fileName, NodeWrapperMapper& wrapperMapper, util::SingletonManager *singletonManager) : m_wrapperMapper(wrapperMapper), m_fileName(fileName)
    {
      m_modelManager = singletonManager->getService<db::ModelManager>();
      m_materialManager = singletonManager->getService<db::MaterialManager>();
      m_renderShaderManager = singletonManager->getService<db::RenderShaderManager>();
      m_textureManager = singletonManager->getService<db::TextureManager>();
    }

    NodeExtractionTraverser::~NodeExtractionTraverser()
    {
    }

    bool NodeExtractionTraverser::preTraverse(sg::TransformNode* treeNode)
    {
      if(!m_wrapperMapper.nodeMap.count(treeNode))
      {
        TransformNodeData *data = new TransformNodeData;
        data->nodeType = treeNode->getNodeType();
        data->nodeName = treeNode->getNodeName();
        data->translation = treeNode->getLocalPosition();
        data->rotation = treeNode->getLocalRotation();
        data->scale = treeNode->getLocalScale();

        m_wrapperMapper.nodeMap[treeNode] = m_wrapperMapper.treeNodes.size();
        m_wrapperMapper.treeNodes.push_back(data);
      }

      return true;
    }

    bool NodeExtractionTraverser::preTraverse(sg::AnimatedTransformNode* treeNode)
    {
      if(!m_wrapperMapper.nodeMap.count(treeNode))
      {
        AnimatedTransformNodeData *data = new AnimatedTransformNodeData;
        data->nodeType = treeNode->getNodeType();
        data->nodeName = treeNode->getNodeName();
        data->translation = treeNode->getLocalPosition();
        data->rotation = treeNode->getLocalRotation();
        data->scale = treeNode->getLocalScale();

        data->boneIndex = treeNode->getBoneIndex();
        data->animationTracks = treeNode->getAnimationTracks();

        m_wrapperMapper.nodeMap[treeNode] = m_wrapperMapper.treeNodes.size();
        m_wrapperMapper.treeNodes.push_back(data);
      }

      return true;
    }

    bool NodeExtractionTraverser::preTraverse(sg::GeoNode* treeNode)
    {
      if(!m_wrapperMapper.nodeMap.count(treeNode))
      {
        GeoNodeData *data = new GeoNodeData;
        data->nodeType = treeNode->getNodeType();
        data->nodeName = treeNode->getNodeName();

        if(!m_wrapperMapper.resourceMap["Meshes"].count(treeNode->getMeshHandle()))
        {
          m_wrapperMapper.resourceMap["Meshes"][treeNode->getMeshHandle()] = m_fileName + "_Mesh";


          db::Mesh *mesh = m_modelManager->getObject(treeNode->getMeshHandle());
        }
        data->meshFilename = m_wrapperMapper.resourceMap["Meshes"][treeNode->getMeshHandle()];

        if(!m_wrapperMapper.resourceMap["Materials"].count(treeNode->getMaterialHandle()))
        {
          m_wrapperMapper.resourceMap["Materials"][treeNode->getMaterialHandle()] = m_fileName + "_Material";

          db::Material *material = m_materialManager->getObject(treeNode->getMaterialHandle());
        }
        data->materialFilename = m_wrapperMapper.resourceMap["Materials"][treeNode->getMeshHandle()];
        
        m_wrapperMapper.nodeMap[treeNode] = m_wrapperMapper.treeNodes.size();
        m_wrapperMapper.treeNodes.push_back(data);
      }

      return true;
    }

    bool NodeExtractionTraverser::preTraverse(sg::AnimatedGeoNode* treeNode)
    {
      if(!m_wrapperMapper.nodeMap.count(treeNode))
      {
        AnimatedGeoNodeData *data = new AnimatedGeoNodeData;
        data->nodeType = treeNode->getNodeType();
        data->nodeName = treeNode->getNodeName();
        data->inverseBindPoseMatrices = treeNode->getInverseBindPoseMatrices();

        if(!m_wrapperMapper.resourceMap["Meshes"].count(treeNode->getMeshHandle()))
        {
          m_wrapperMapper.resourceMap["Meshes"][treeNode->getMeshHandle()] = m_fileName + "_Mesh";

          db::Mesh *mesh = m_modelManager->getObject(treeNode->getMeshHandle());
        }
        data->meshFilename = m_wrapperMapper.resourceMap["Meshes"][treeNode->getMeshHandle()];

        if(!m_wrapperMapper.resourceMap["Materials"].count(treeNode->getMaterialHandle()))
        {
          m_wrapperMapper.resourceMap["Materials"][treeNode->getMaterialHandle()] = m_fileName + "_Material";

          db::Material *material = m_materialManager->getObject(treeNode->getMaterialHandle());
        }
        data->materialFilename = m_wrapperMapper.resourceMap["Materials"][treeNode->getMeshHandle()];

        m_wrapperMapper.nodeMap[treeNode] = m_wrapperMapper.treeNodes.size();
        m_wrapperMapper.treeNodes.push_back(data);
      }

      return true;
    }

    bool NodeExtractionTraverser::preTraverse(sg::BillboardNode* treeNode)
    {
      if(!m_wrapperMapper.nodeMap.count(treeNode))
      {
        BillboardNodeData *data = new BillboardNodeData;
        data->nodeType = treeNode->getNodeType();
        data->nodeName = treeNode->getNodeName();
        data->animCount = treeNode->getAnimationCount();
        data->animNumber = treeNode->getAnimationNumber();
        data->translate = treeNode->getPosition();
        data->scale = treeNode->getScale();
        data->texStart = treeNode->getTextureStart();
        data->texEnd = treeNode->getTextureEnd();

        if(!m_wrapperMapper.resourceMap["Textures"].count(treeNode->getTextureHandle()))
        {
          m_wrapperMapper.resourceMap["Textures"][treeNode->getTextureHandle()] = m_fileName + "_Texture";

          db::Mesh *mesh = m_modelManager->getObject(treeNode->getTextureHandle());
        }
        data->textureFilename = m_wrapperMapper.resourceMap["Textures"][treeNode->getTextureHandle()];

        m_wrapperMapper.nodeMap[treeNode] = m_wrapperMapper.treeNodes.size();
        m_wrapperMapper.treeNodes.push_back(data);
      }

      return true;
    }

    bool NodeExtractionTraverser::preTraverse(sg::LODNode* treeNode)
    {
      if(!m_wrapperMapper.nodeMap.count(treeNode))
      {
        LODNodeData *data = new LODNodeData;
        data->nodeType = treeNode->getNodeType();
        data->nodeName = treeNode->getNodeName();
        data->lodLevel = treeNode->getLODLevel();
        data->position = treeNode->getPosition();

        m_wrapperMapper.nodeMap[treeNode] = m_wrapperMapper.treeNodes.size();
        m_wrapperMapper.treeNodes.push_back(data);
      }

      return true;
    }

    bool NodeExtractionTraverser::preTraverse(sg::LightNode* treeNode)
    {
      if(!m_wrapperMapper.nodeMap.count(treeNode))
      {
        LightNodeData *data = new LightNodeData;
        data->nodeType = treeNode->getNodeType();
        data->nodeName = treeNode->getNodeName();

        data->color = treeNode->getColor();

        data->intensity = treeNode->getIntensity();

        data->spotLightExponent = treeNode->getSpotLightExponent();
        data->spotLightCutoff = treeNode->getSpotLightCutoff();

        data->constAttenuation = treeNode->getConstAttenuation();
        data->linearAttenuation = treeNode->getLinearAttenuation();
        data->quadricAttenuation = treeNode->getQuadricAttenuation();

        data->lightType = treeNode->getLightType();

        m_wrapperMapper.nodeMap[treeNode] = m_wrapperMapper.treeNodes.size();
        m_wrapperMapper.treeNodes.push_back(data);
      }

      return true;
    }

    bool NodeExtractionTraverser::preTraverse(sg::ShadowLightNode* treeNode)
    {
      if(!m_wrapperMapper.nodeMap.count(treeNode))
      {
        ShadowLightNodeData *data = new ShadowLightNodeData;
        data->nodeType = treeNode->getNodeType();
        data->nodeName = treeNode->getNodeName();

        data->color = treeNode->getColor();

        data->intensity = treeNode->getIntensity();

        data->spotLightExponent = treeNode->getSpotLightExponent();
        data->spotLightCutoff = treeNode->getSpotLightCutoff();

        data->constAttenuation = treeNode->getConstAttenuation();
        data->linearAttenuation = treeNode->getLinearAttenuation();
        data->quadricAttenuation = treeNode->getQuadricAttenuation();

        data->projectionMatrix = treeNode->getShadowProjectionMatrix();

        data->lightType = treeNode->getLightType();

        m_wrapperMapper.nodeMap[treeNode] = m_wrapperMapper.treeNodes.size();
        m_wrapperMapper.treeNodes.push_back(data);
      }

      return true;
    }

    bool NodeExtractionTraverser::preTraverse(sg::ParticleNode* treeNode)
    {
      if(!m_wrapperMapper.nodeMap.count(treeNode))
      {
        ParticleNodeData *data = new ParticleNodeData;
        data->nodeType = treeNode->getNodeType();
        data->nodeName = treeNode->getNodeName();
        data->trfMatrix = treeNode->getTransformationMatrix();

        m_wrapperMapper.nodeMap[treeNode] = m_wrapperMapper.treeNodes.size();
        m_wrapperMapper.treeNodes.push_back(data);
      }

      return true;
    }
  }
}

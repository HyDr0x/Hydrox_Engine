#include "Loader/HEFLoader.h"

#include <fstream>

#include <Utilities/Signals/EventManager.h>
#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/PrimitiveGenerator/CubeGenerator.h>

#include <SceneGraph/TreeNodes/GroupNode.h>
#include <SceneGraph/TreeNodes/GeoNode.h>
#include <SceneGraph/TreeNodes/AnimatedGeoNode.h>
#include <SceneGraph/TreeNodes/TransformNode.h>
#include <SceneGraph/TreeNodes/AnimatedTransformNode.h>
#include <SceneGraph/TreeNodes/BillboardNode.h>
#include <SceneGraph/TreeNodes/LODNode.h>
#include <SceneGraph/TreeNodes/LightNode.h>
#include <SceneGraph/TreeNodes/ShadowLightNode.h>
#include <SceneGraph/TreeNodes/ParticleNode.h>

#include <SceneGraph/Scene/Scene.h>

#include <DataBase/Texture2D.h>
#include <DataBase/Material.h>
#include <DataBase/Mesh.h>

#include "Loader/ILDevilLoader.h"
#include "Loader/RenderShaderLoader.h"
#include "Loader/MaterialLoader.h"
#include "Loader/MeshLoader.h"

namespace he
{
  namespace loader
  {
    sg::TreeNode* createGeoNode()
    {
      return new sg::GeoNode();
    }

    HEFLoader::HEFLoader()
    {
      m_factory.registerCreateFunction(sg::GEONODE, );
    }

    sg::Scene* HEFLoader::load(std::string path, std::string filename, util::SingletonManager *singletonManager)
    {
      m_rootNode = nullptr;

      readFromFile(path, filename, singletonManager);
      createSceneNodes(singletonManager->getService<util::EventManager>());
      linkSceneNodes();
      findRootNode();

      return new sg::Scene(m_rootNode);
    }

    void HEFLoader::readFromFile(std::string path, std::string filename, util::SingletonManager *singletonManager)
    {
      db::ModelManager *modelManager = singletonManager->getService<db::ModelManager>();

      if(path.back() != '/')
      {
        path.push_back('/');
      }

      std::ifstream fileStream;

      fileStream.open(path + filename, std::ifstream::in | std::ofstream::binary);

      if(fileStream.fail())
      {
        return;
      }

      unsigned int treeNodeSize;
      fileStream >> treeNodeSize;
      m_wrapperMapper.treeNodes.resize(treeNodeSize);

      for(unsigned int i = 0; i < m_wrapperMapper.treeNodes.size(); i++)
      {
        fileStream >> m_wrapperMapper.treeNodes[i]->nodeType;
        switch(m_wrapperMapper.treeNodes[i]->nodeType)
        {

        }
        m_wrapperMapper.treeNodes[i] = new TreeNodeData;
        fileStream >> *m_wrapperMapper.treeNodes[i];
      }

      std::string resourceFilename;
      MeshLoader meshLoader = MeshLoader(singletonManager);

      unsigned int meshMapSize;
      fileStream >> meshMapSize;
      std::getline(fileStream, std::string());
      
      for(unsigned int i = 0; i < meshMapSize; i++)
      {
        std::getline(fileStream, resourceFilename);
        m_wrapperMapper.resourceMap["Meshes"][resourceFilename] = meshLoader.loadResource(resourceFilename);
      }

      unsigned int materialMapSize;
      fileStream >> materialMapSize;
      std::getline(fileStream, std::string());
      MaterialLoader materialLoader(singletonManager);
      for(unsigned int i = 0; i < materialMapSize; i++)
      {
        std::string materialFilename;
        
        std::getline(fileStream, materialFilename);
        
        m_wrapperMapper.materialMap[i] = materialLoader.loadResource(path + materialFilename);
      }

      unsigned int billboardTextureMapSize;
      fileStream >> billboardTextureMapSize;
      std::getline(fileStream, std::string());
      ILDevilLoader ilDevilLoader(singletonManager);
      for(unsigned int i = 0; i < billboardTextureMapSize; i++)
      {
        std::string billboardTextureFilename;
        
        std::getline(fileStream, billboardTextureFilename);

        m_wrapperMapper.billboardTextureMap[i] = ilDevilLoader.loadResource(billboardTextureFilename);
      }

      fileStream.close();
    }

    void HEFLoader::createSceneNodes(util::EventManager *eventManager)
    {
      for(unsigned int i = 0; i < m_wrapperMapper.geoNodes.size(); i++)
      {
        GeoNodeData& data = m_wrapperMapper.geoNodes[i];
        m_wrapperMapper.geoNodeMap[i] = new sg::GeoNode(eventManager, m_wrapperMapper.meshMap[data.meshIndex], m_wrapperMapper.materialMap[data.materialIndex], data.nodeName);
      }

      for(unsigned int i = 0; i < m_wrapperMapper.animatedGeoNodes.size(); i++)
      {
        AnimatedGeoNodeData& data = m_wrapperMapper.animatedGeoNodes[i];
        m_wrapperMapper.animatedGeoNodeMap[i] = new sg::AnimatedGeoNode(data.inverseBindPoseMatrices, eventManager, m_wrapperMapper.meshMap[data.meshIndex], m_wrapperMapper.materialMap[data.materialIndex], data.nodeName);
      }

      for(unsigned int i = 0; i < m_wrapperMapper.transformNodes.size(); i++)
      {
        TransformNodeData& data = m_wrapperMapper.transformNodes[i];
        m_wrapperMapper.transformNodeMap[i] = new sg::TransformNode(data.translation, data.scale, data.rotation, data.nodeName);
      }

      for(unsigned int i = 0; i < m_wrapperMapper.animatedTransformNodes.size(); i++)
      {
        AnimatedTransformNodeData& data = m_wrapperMapper.animatedTransformNodes[i];
        m_wrapperMapper.animatedTransformNodeMap[i] = new sg::AnimatedTransformNode(data.animationTracks, data.nodeName);
        m_wrapperMapper.animatedTransformNodeMap[i]->setBoneIndex(data.boneIndex);
      }

      for(unsigned int i = 0; i < m_wrapperMapper.billboardNodes.size(); i++)
      {
        BillboardNodeData& data = m_wrapperMapper.billboardNodes[i];
        m_wrapperMapper.billboardNodeMap[i] = new sg::BillboardNode(eventManager, m_wrapperMapper.billboardTextureMap[data.textureIndex], data.animNumber, data.texStart, data.texEnd, data.nodeName);
      }

      for(unsigned int i = 0; i < m_wrapperMapper.lodNodes.size(); i++)
      {
        LODNodeData& data = m_wrapperMapper.lodNodes[i];
        m_wrapperMapper.lodNodeMap[i] = new sg::LODNode(data.position, data.lodLevel, data.nodeName);
      }

      for(unsigned int i = 0; i < m_wrapperMapper.lightNodes.size(); i++)
      {
        LightNodeData& data = m_wrapperMapper.lightNodes[i];
        m_wrapperMapper.lightNodeMap[i] = new sg::LightNode(data.lightType, eventManager, data.nodeName);
        m_wrapperMapper.lightNodeMap[i]->setColor(data.color);
        m_wrapperMapper.lightNodeMap[i]->setIntensity(data.intensity);
        m_wrapperMapper.lightNodeMap[i]->setSpotLightCutoff(data.spotLightCutoff);
        m_wrapperMapper.lightNodeMap[i]->setSpotLightExponent(data.spotLightExponent);
        m_wrapperMapper.lightNodeMap[i]->setConstAttenuation(data.constAttenuation);
        m_wrapperMapper.lightNodeMap[i]->setLinearAttenuation(data.linearAttenuation);
        m_wrapperMapper.lightNodeMap[i]->setQuadricAttenuation(data.quadricAttenuation);
      }

      for(unsigned int i = 0; i < m_wrapperMapper.lightNodes.size(); i++)
      {
        ShadowLightNodeData& data = m_wrapperMapper.shadowLightNodes[i];
        m_wrapperMapper.shadowLightNodeMap[i] = new sg::ShadowLightNode(data.lightType, eventManager, data.nodeName);
        m_wrapperMapper.shadowLightNodeMap[i]->setColor(data.color);
        m_wrapperMapper.shadowLightNodeMap[i]->setIntensity(data.intensity);
        m_wrapperMapper.shadowLightNodeMap[i]->setSpotLightCutoff(data.spotLightCutoff);
        m_wrapperMapper.shadowLightNodeMap[i]->setSpotLightExponent(data.spotLightExponent);
        m_wrapperMapper.shadowLightNodeMap[i]->setConstAttenuation(data.constAttenuation);
        m_wrapperMapper.shadowLightNodeMap[i]->setLinearAttenuation(data.linearAttenuation);
        m_wrapperMapper.shadowLightNodeMap[i]->setQuadricAttenuation(data.quadricAttenuation);
      }

      for(unsigned int i = 0; i < m_wrapperMapper.particleNodes.size(); i++)
      {
        ParticleNodeData& data = m_wrapperMapper.particleNodes[i];
        m_wrapperMapper.particleNodeMap[i] = new sg::ParticleNode(eventManager, data.nodeName);
      }
    }

    void HEFLoader::linkSceneNodes()
    {
      sg::GroupNode *groupNode = nullptr;
      sg::TreeNode *treeNode = nullptr;

      for(unsigned int i = 0; i < m_wrapperMapper.geoNodeMap.size(); i++)
      {
        GeoNodeData& data = m_wrapperMapper.geoNodes[i];

        findNode(&groupNode, data.parentIndex, data.parentNodeType);
        m_wrapperMapper.geoNodeMap[i]->setParent(groupNode);

        findNode(&treeNode, data.nextSiblingIndex, data.nextSiblingNodeType);
        m_wrapperMapper.geoNodeMap[i]->setNextSibling(treeNode);
      }

      for(unsigned int i = 0; i < m_wrapperMapper.animatedGeoNodes.size(); i++)
      {
        AnimatedGeoNodeData& data = m_wrapperMapper.animatedGeoNodes[i];
        
        findNode(&groupNode, data.parentIndex, data.parentNodeType);
        m_wrapperMapper.animatedGeoNodeMap[i]->setParent(groupNode);

        findNode(&treeNode, data.nextSiblingIndex, data.nextSiblingNodeType);
        m_wrapperMapper.animatedGeoNodeMap[i]->setNextSibling(treeNode);
      }

      for(unsigned int i = 0; i < m_wrapperMapper.transformNodes.size(); i++)
      {
        TransformNodeData& data = m_wrapperMapper.transformNodes[i];
        
        findNode(&treeNode, data.firstChildIndex, data.firstChildNodeType);
        m_wrapperMapper.transformNodeMap[i]->setFirstChild(treeNode);

        findNode(&groupNode, data.parentIndex, data.parentNodeType);
        m_wrapperMapper.transformNodeMap[i]->setParent(groupNode);

        findNode(&treeNode, data.nextSiblingIndex, data.nextSiblingNodeType);
        m_wrapperMapper.transformNodeMap[i]->setNextSibling(treeNode);
      }

      for(unsigned int i = 0; i < m_wrapperMapper.animatedTransformNodes.size(); i++)
      {
        AnimatedTransformNodeData& data = m_wrapperMapper.animatedTransformNodes[i];

        if(data.animatedMeshIndex != ~0)
        {
          m_wrapperMapper.animatedTransformNodeMap[i]->setSkinnedMesh(m_wrapperMapper.animatedGeoNodeMap[data.animatedMeshIndex]);
        }

        findNode(&treeNode, data.firstChildIndex, data.firstChildNodeType);
        m_wrapperMapper.animatedTransformNodeMap[i]->setFirstChild(treeNode);

        findNode(&groupNode, data.parentIndex, data.parentNodeType);
        m_wrapperMapper.animatedTransformNodeMap[i]->setParent(groupNode);

        findNode(&treeNode, data.nextSiblingIndex, data.nextSiblingNodeType);
        m_wrapperMapper.animatedTransformNodeMap[i]->setNextSibling(treeNode);
      }

      for(unsigned int i = 0; i < m_wrapperMapper.billboardNodes.size(); i++)
      {
        BillboardNodeData& data = m_wrapperMapper.billboardNodes[i];
        
        findNode(&groupNode, data.parentIndex, data.parentNodeType);
        m_wrapperMapper.billboardNodeMap[i]->setParent(groupNode);

        findNode(&treeNode, data.nextSiblingIndex, data.nextSiblingNodeType);
        m_wrapperMapper.billboardNodeMap[i]->setNextSibling(treeNode);
      }

      for(unsigned int i = 0; i < m_wrapperMapper.lodNodes.size(); i++)
      {
        LODNodeData& data = m_wrapperMapper.lodNodes[i];
        
        findNode(&treeNode, data.firstChildIndex, data.firstChildNodeType);
        m_wrapperMapper.lodNodeMap[i]->setFirstChild(treeNode);

        findNode(&groupNode, data.parentIndex, data.parentNodeType);
        m_wrapperMapper.lodNodeMap[i]->setParent(groupNode);

        findNode(&treeNode, data.nextSiblingIndex, data.nextSiblingNodeType);
        m_wrapperMapper.lodNodeMap[i]->setNextSibling(treeNode);
      }

      for(unsigned int i = 0; i < m_wrapperMapper.lightNodes.size(); i++)
      {
        LightNodeData& data = m_wrapperMapper.lightNodes[i];
        
        findNode(&groupNode, data.parentIndex, data.parentNodeType);
        m_wrapperMapper.lightNodeMap[i]->setParent(groupNode);

        findNode(&treeNode, data.nextSiblingIndex, data.nextSiblingNodeType);
        m_wrapperMapper.lightNodeMap[i]->setNextSibling(treeNode);
      }

      for(unsigned int i = 0; i < m_wrapperMapper.shadowLightNodes.size(); i++)
      {
        ShadowLightNodeData& data = m_wrapperMapper.shadowLightNodes[i];

        findNode(&groupNode, data.parentIndex, data.parentNodeType);
        m_wrapperMapper.shadowLightNodeMap[i]->setParent(groupNode);

        findNode(&treeNode, data.nextSiblingIndex, data.nextSiblingNodeType);
        m_wrapperMapper.shadowLightNodeMap[i]->setNextSibling(treeNode);
      }

      for(unsigned int i = 0; i < m_wrapperMapper.particleNodes.size(); i++)
      {
        ParticleNodeData& data = m_wrapperMapper.particleNodes[i];
        
        findNode(&groupNode, data.parentIndex, data.parentNodeType);
        m_wrapperMapper.particleNodeMap[i]->setParent(groupNode);

        findNode(&treeNode, data.nextSiblingIndex, data.nextSiblingNodeType);
        m_wrapperMapper.particleNodeMap[i]->setNextSibling(treeNode);
      }
    }

    void HEFLoader::findRootNode()
    {
      sg::GroupNode *newNode = m_wrapperMapper.transformNodeMap[0];//pick a random node
      sg::GroupNode *oldNode = nullptr;

      while(newNode != nullptr)
      {
        oldNode = newNode;
        newNode = oldNode->getParent();
      }

      m_rootNode = oldNode;
    }

    void HEFLoader::findNode(sg::TreeNode **treeNode, unsigned int index, NodeType nodeType)
    {
      if(index != ~0)
      {
        switch(nodeType)
        {
          case ANIMATEDGEONODE:
            *treeNode = m_wrapperMapper.animatedGeoNodeMap[index];
            break;
          case GEONODE:
            *treeNode = m_wrapperMapper.geoNodeMap[index];
            break;
          case ANIMATEDTRANSFORMNODE:
            *treeNode = m_wrapperMapper.animatedTransformNodeMap[index];
            break;
          case TRANSFORMNODE:
            *treeNode = m_wrapperMapper.transformNodeMap[index];
            break;
          case BILLBOARDNODE:
            *treeNode = m_wrapperMapper.billboardNodeMap[index];
            break;
          case LODNODE:
            *treeNode = m_wrapperMapper.lodNodeMap[index];
            break;
          case LIGHTNODE:
            *treeNode = m_wrapperMapper.lightNodeMap[index];
            break;
          case SHADOWLIGHTNODE:
            *treeNode = m_wrapperMapper.shadowLightNodeMap[index];
            break;
          case PARTICLENODE:
            *treeNode = m_wrapperMapper.particleNodeMap[index];
            break;
        }
      }
      else
      {
        *treeNode = nullptr;
      }
    }

    void HEFLoader::findNode(sg::GroupNode **groupNode, unsigned int index, NodeType nodeType)
    {
      if(index != ~0)
      {
        switch(nodeType)
        {
          case ANIMATEDTRANSFORMNODE:
            *groupNode = m_wrapperMapper.animatedTransformNodeMap[index];
            break;
          case TRANSFORMNODE:
            *groupNode = m_wrapperMapper.transformNodeMap[index];
            break;
          case LODNODE:
            *groupNode = m_wrapperMapper.lodNodeMap[index];
            break;
        }
      }
      else
      {
        *groupNode = nullptr;
      }
    }
  }
}
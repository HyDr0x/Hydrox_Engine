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
#include <SceneGraph/TreeNodes/ParticleNode.h>

#include <SceneGraph/Scene/Scene.h>

#include <Renderer/Resources/Texture2D.h>
#include <Renderer/Resources/Material.h>
#include <Renderer/Resources/Mesh.h>

#include "Loader/ILDevilLoader.h"
#include "Loader/RenderShaderLoader.h"
#include "Loader/MaterialLoader.h"

namespace he
{
  namespace loader
  {
    sg::Scene* HEFLoader::load(std::string path, std::string filename, util::SingletonManager *singletonManager, util::EventManager *eventManager)
    {
      m_wrapperMapper = NodeWrapperMapper();
      m_rootNode = nullptr;

      readFromFile(path, filename, singletonManager);
      createSceneNodes(eventManager);
      linkSceneNodes();
      findRootNode();

      return new sg::Scene(m_rootNode);
    }

    void HEFLoader::readFromFile(std::string path, std::string filename, util::SingletonManager *singletonManager)
    {
      renderer::ModelManager *modelManager = singletonManager->getService<renderer::ModelManager>();

      if(path.back() != '/')
      {
        path.push_back('/');
      }

      std::ifstream fileStream;

      fileStream.open(path + filename, std::ifstream::in);

      if(fileStream.fail())
      {
        return;
      }

      unsigned int vectorSize;
      fileStream >> vectorSize;
      m_wrapperMapper.geoNodes.resize(vectorSize);
      for(unsigned int i = 0; i < m_wrapperMapper.geoNodes.size(); i++)
      {
        fileStream >> m_wrapperMapper.geoNodes[i];
      }

      fileStream >> vectorSize;
      m_wrapperMapper.animatedGeoNodes.resize(vectorSize);
      for(unsigned int i = 0; i < m_wrapperMapper.animatedGeoNodes.size(); i++)
      {
        fileStream >> m_wrapperMapper.animatedGeoNodes[i];
      }

      fileStream >> vectorSize;
      m_wrapperMapper.transformNodes.resize(vectorSize);
      for(unsigned int i = 0; i < m_wrapperMapper.transformNodes.size(); i++)
      {
        fileStream >> m_wrapperMapper.transformNodes[i];
      }

      fileStream >> vectorSize;
      m_wrapperMapper.animatedTransformNodes.resize(vectorSize);
      for(unsigned int i = 0; i < m_wrapperMapper.animatedTransformNodes.size(); i++)
      {
        fileStream >> m_wrapperMapper.animatedTransformNodes[i];
      }

      fileStream >> vectorSize;
      m_wrapperMapper.billboardNodes.resize(vectorSize);
      for(unsigned int i = 0; i < m_wrapperMapper.billboardNodes.size(); i++)
      {
        fileStream >> m_wrapperMapper.billboardNodes[i];
      }

      fileStream >> vectorSize;
      m_wrapperMapper.lodNodes.resize(vectorSize);
      for(unsigned int i = 0; i < m_wrapperMapper.lodNodes.size(); i++)
      {
        fileStream >> m_wrapperMapper.lodNodes[i];
      }

      fileStream >> vectorSize;
      m_wrapperMapper.lightNodes.resize(vectorSize);
      for(unsigned int i = 0; i < m_wrapperMapper.lightNodes.size(); i++)
      {
        fileStream >> m_wrapperMapper.lightNodes[i];
      }

      fileStream >> vectorSize;
      m_wrapperMapper.particleNodes.resize(vectorSize);
      for(unsigned int i = 0; i < m_wrapperMapper.particleNodes.size(); i++)
      {
        fileStream >> m_wrapperMapper.particleNodes[i];
      }

      unsigned int meshMapSize;
      fileStream >> meshMapSize;

      for(unsigned int i = 0; i < meshMapSize; i++)
      {
        util::Vector<float, 3> bbMin;
        util::Vector<float, 3> bbMax;
        GLuint primitiveType;
        GLuint primitiveCount;
        GLuint indexCount;
        GLuint vertexCount;
        GLuint vertexDeclarationFlags;
        GLuint vertexStride;
        GLuint vboSize;

        fileStream >> bbMax;
        fileStream >> bbMin;
        fileStream >> primitiveType;
        fileStream >> primitiveCount;
        fileStream >> indexCount;
        fileStream >> vertexCount;
        fileStream >> vertexDeclarationFlags;
        fileStream >> vertexStride;
        fileStream >> vboSize;

        std::vector<renderer::Mesh::indexType> indexData(indexCount);
        for(unsigned int j = 0; j < indexData.size(); j++)
        {
          fileStream >> indexData[j];
        }

        std::vector<GLubyte> geometryData(vboSize);
        for(unsigned int j = 0; j < geometryData.size(); j++)
        {
          GLuint t;
          fileStream >> t;
          geometryData[j] = (GLubyte)t;
        }

        m_wrapperMapper.meshMap[i] = modelManager->addObject(renderer::Mesh(renderer::AABB(bbMin, bbMax), primitiveType, primitiveCount, vertexCount, vertexStride, vertexDeclarationFlags, geometryData, indexData));
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
        m_wrapperMapper.lightNodeMap[i] = new sg::LightNode(data.nodeName);
      }

      for(unsigned int i = 0; i < m_wrapperMapper.particleNodes.size(); i++)
      {
        ParticleNodeData& data = m_wrapperMapper.particleNodes[i];
        m_wrapperMapper.particleNodeMap[i] = new sg::ParticleNode(data.nodeName);
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
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
#include <SceneGraph/Scene/TreeNodeAllocator.h>

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
    HEFLoader::HEFLoader(unsigned int nodeBlockSize) : m_allocator(nodeBlockSize), m_rootNode(~0)
    {
    }

    sg::Scene* HEFLoader::load(std::string path, std::string filename, util::SingletonManager *singletonManager)
    {
      m_rootNode = ~0;

      readFromFile(path, filename, singletonManager);
      findRootNode();

      return new sg::Scene(m_allocator, m_rootNode);
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

      m_allocator.readFromStream(fileStream);

      std::string resourceFilename;
      MeshLoader meshLoader = MeshLoader(singletonManager);

      unsigned int meshMapSize;
      fileStream >> meshMapSize;
      std::getline(fileStream, std::string());
      
      for(unsigned int i = 0; i < meshMapSize; i++)
      {
        std::getline(fileStream, resourceFilename);
        m_resourceMap["Meshes"][resourceFilename] = meshLoader.loadResource(resourceFilename);
      }

      std::string materialFilename;
      MaterialLoader materialLoader(singletonManager);

      unsigned int materialMapSize;
      fileStream >> materialMapSize;
      std::getline(fileStream, std::string());
      
      for(unsigned int i = 0; i < materialMapSize; i++)
      {
        std::getline(fileStream, materialFilename);
        m_resourceMap["Materials"][materialFilename] = materialLoader.loadResource(path + materialFilename);
      }

      std::string billboardTextureFilename;
      ILDevilLoader ilDevilLoader(singletonManager);

      unsigned int billboardTextureMapSize;
      fileStream >> billboardTextureMapSize;
      std::getline(fileStream, std::string());
      
      for(unsigned int i = 0; i < billboardTextureMapSize; i++)
      {
        std::getline(fileStream, billboardTextureFilename);
        m_resourceMap["Textures"][billboardTextureFilename] = ilDevilLoader.loadResource(billboardTextureFilename);
      }

      fileStream.close();
    }

    void HEFLoader::findRootNode()
    {
      sg::NodeIndex newNodeIndex = m_allocator[0].getNodeIndex();//pick a random node
      sg::NodeIndex oldNodeIndex = ~0;

      while(newNodeIndex != ~0)
      {
        oldNodeIndex = newNodeIndex;
        newNodeIndex = m_allocator[oldNodeIndex].getParent();
      }

      m_rootNode = oldNodeIndex;
    }
  }
}
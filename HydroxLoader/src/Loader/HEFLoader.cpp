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
    HEFLoader::HEFLoader(float errorRate, float maxDistance, float maxAngle, unsigned int nodeBlockSize) : 
      m_errorRate(errorRate),
      m_maxDistance(maxDistance),
      m_maxAngle(maxAngle),
      m_allocator(nodeBlockSize), 
      m_rootNode(~0)
    {
    }

    sg::Scene* HEFLoader::load(std::string path, std::string filename, util::SingletonManager *singletonManager)
    {
      m_rootNode = ~0;

      readFromFile(path, filename, singletonManager);

      return new sg::Scene(m_allocator, m_rootNode);
    }

    void HEFLoader::readFromFile(std::string path, std::string filename, util::SingletonManager *singletonManager)
    {
      util::SharedPointer<db::ModelManager> modelManager = singletonManager->getService<db::ModelManager>();

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

      std::string resourceFilename;
      MeshLoader meshLoader = MeshLoader(m_errorRate, m_maxDistance, m_maxAngle, singletonManager);

      unsigned int meshMapSize;
      fileStream >> meshMapSize;
      std::getline(fileStream, std::string());
      
      for(unsigned int i = 0; i < meshMapSize; i++)
      {
        std::getline(fileStream, resourceFilename);
        util::ResourceHandle handle = meshLoader.loadResource(path + resourceFilename + ".mesh");
        m_resourceMap["Meshes"][resourceFilename] = handle;
      }

      MaterialLoader materialLoader(singletonManager);

      unsigned int materialMapSize;
      fileStream >> materialMapSize;
      std::getline(fileStream, std::string());
      
      for(unsigned int i = 0; i < materialMapSize; i++)
      {
        std::getline(fileStream, resourceFilename);
        util::ResourceHandle handle = materialLoader.loadResource(path + resourceFilename + ".material");
        m_resourceMap["Materials"][resourceFilename] = handle;
      }

      ILDevilLoader ilDevilLoader(singletonManager);

      unsigned int billboardTextureMapSize;
      fileStream >> billboardTextureMapSize;
      std::getline(fileStream, std::string());
      
      for(unsigned int i = 0; i < billboardTextureMapSize; i++)
      {
        std::getline(fileStream, resourceFilename);
        util::ResourceHandle handle = ilDevilLoader.loadResource(path + resourceFilename + ".png");
        m_resourceMap["Textures"][resourceFilename] = handle;
      }

      read(fileStream, m_allocator, m_resourceMap, singletonManager->getService<util::EventManager>());

      unsigned int nodeType;
      unsigned int nodeIndex;
      fileStream >> nodeType;
      fileStream >> nodeIndex;

      m_rootNode = sg::NodeIndex(nodeIndex, (sg::NodeType)nodeType);

      fileStream.close();
    }

    void HEFLoader::read(std::istream& stream, sg::TreeNodeAllocator& allocator, std::map<std::string, std::map<std::string, util::ResourceHandle>> resourceHandles, util::SharedPointer<util::EventManager> eventManager)
    {
      unsigned int nodeTypeNumber;
      stream >> nodeTypeNumber;

      unsigned int nodeBlockSize;
      stream >> nodeBlockSize;

      for(unsigned int i = 0; i < nodeTypeNumber; i++)
      {
        unsigned int nodeBlockNumber;
        stream >> nodeBlockNumber;

        unsigned int exactNodeBlockSize;
        stream >> exactNodeBlockSize;

        unsigned int nodeCounter = 0;

        for(unsigned int j = 0; j < nodeBlockNumber && nodeCounter < exactNodeBlockSize; j++)
        {
          for(unsigned int k = 0; k < nodeBlockSize && nodeCounter < exactNodeBlockSize; k++, nodeCounter++)
          {
            unsigned int index;
            unsigned int type;
            stream >> type;
            stream >> index;
            sg::NodeIndex nodeIndex = sg::NodeIndex(index, (sg::NodeType)type);
            sg::TreeNode& node = allocator.insert(nodeIndex);
            node.read(stream, eventManager, resourceHandles);
          }
        }
      }
    }
  }
}
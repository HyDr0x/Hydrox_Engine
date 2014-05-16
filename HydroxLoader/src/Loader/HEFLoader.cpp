#include "Loader/HEFLoader.h"

#include <fstream>

#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/PrimitiveGenerator/CubeGenerator.h>

#include <SceneGraph/TreeNodes/GroupNode.h>
#include <SceneGraph/TreeNodes/AnimatedGeoNode.h>
#include <SceneGraph/TreeNodes/GeoNode.h>
#include <SceneGraph/TreeNodes/LODNode.h>
#include <SceneGraph/TreeNodes/TransformNode.h>
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
    sg::Scene* HEFLoader::load(std::string path, std::string filename, util::SingletonManager *singletonManager)
    {
      m_wrapperMapper = NodeWrapperMapper();

      readFromFile(path, filename, singletonManager);

      return nullptr;
    }

    void HEFLoader::readFromFile(std::string path, std::string filename, util::SingletonManager *singletonManager)
    {
      renderer::ModelManager *modelManager = singletonManager->getService<renderer::ModelManager>();

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

        fileStream >> bbMin;
        fileStream >> bbMax;
        fileStream >> primitiveType;
        fileStream >> primitiveCount;
        fileStream >> indexCount;
        fileStream >> vertexCount;
        fileStream >> vertexDeclarationFlags;
        fileStream >> vertexStride;
        fileStream >> vboSize;

        std::vector<renderer::Mesh::indexType> indexData(indexCount);
        for(unsigned int i = 0; i < indexData.size(); i++)
        {
          fileStream >> indexData[i];
        }

        std::vector<GLubyte> geometryData(vboSize);
        for(unsigned int i = 0; i < geometryData.size(); i++)
        {
          fileStream >> geometryData[i];
          if(i==364)
          {
            int g = 0;
          }
        }

        m_wrapperMapper.meshMap[i] = modelManager->addObject(renderer::Mesh(renderer::AABB(bbMin, bbMax), primitiveType, primitiveCount, vertexCount, vertexStride, vertexDeclarationFlags, geometryData, indexData));
      }

      unsigned int materialMapSize;
      fileStream >> materialMapSize;

      MaterialLoader materialLoader(singletonManager);
      for(unsigned int i = 0; i < materialMapSize; i++)
      {
        std::string materialFilename;
        fileStream >> materialFilename;
        
        m_wrapperMapper.materialMap[i] = materialLoader.loadResource(path + materialFilename);
      }

      unsigned int billboardTextureMapSize;
      fileStream >> billboardTextureMapSize;

      ILDevilLoader ilDevilLoader(singletonManager);
      for(unsigned int i = 0; i < billboardTextureMapSize; i++)
      {
        std::string billboardTextureFilename;
        fileStream >> billboardTextureFilename;

        m_wrapperMapper.billboardTextureMap[i] = ilDevilLoader.loadResource(billboardTextureFilename);
      }

      fileStream.close();
    }
  }
}
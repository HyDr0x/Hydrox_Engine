#include "Loader/MeshLoader.h"

#include <sstream>
#include <fstream>
#include <iostream>

#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/PrimitiveGenerator/CubeGenerator.h>

#include <DataBase/Material.h>

#include "Loader/RenderShaderLoader.h"
#include "Loader/ILDevilLoader.h"

namespace he
{
  namespace loader
  {
    MeshLoader::MeshLoader(float errorRate, float maxDistance, float maxAngle, util::SingletonManager *singletonManager) : 
      ResourceLoader(singletonManager),
      m_modelManager(singletonManager->getService<db::ModelManager>()),
      m_errorRate(errorRate),
      m_maxDistance(maxDistance),
      m_maxAngle(maxAngle)
    {
    }

    util::ResourceHandle MeshLoader::loadResource(std::string filename)
    {
      if(filename == std::string())
      {
        return util::ResourceHandle();
      }

      util::ResourceHandle meshHandle;

      std::ifstream file(filename, std::ifstream::in | std::ifstream::binary);
      std::string line;

      if(file.is_open())
      {
        MeshMetaData meshData;
        file.read((char*)&meshData, sizeof(meshData));

        std::vector<db::Mesh::indexType> indexData(meshData.indexCount);
        std::vector<GLubyte> geometryData(meshData.vboSize);

        file.read((char*)&indexData[0], sizeof(indexData[0]) * meshData.indexCount);
        file.read((char*)&geometryData[0], sizeof(geometryData[0]) * meshData.vboSize);

        meshHandle = m_modelManager->addObject(db::Mesh(db::AABB(meshData.bbMin, meshData.bbMax), meshData.primitiveType, meshData.primitiveCount, meshData.vertexCount, meshData.vertexStride, meshData.vertexDeclaration, geometryData, indexData));
      }
      else//wrong filename or file does not exist
      {
        file.close();

        std::cerr << "Error: couldn't open mesh source file " << filename << "." << std::endl;

        return getDefaultResource();
      }

      file.close();

      return meshHandle;
    }

    util::ResourceHandle MeshLoader::getDefaultResource() const
    {
      std::vector<util::vec3f> positions;
      std::vector<util::vec3f> normals;
      std::vector<db::Mesh::indexType> indices;
      util::CubeGenerator::generateCube(positions, indices, normals);

      util::PointCloudGenerator generator;
      std::vector<util::Cache> caches = generator.generateCaches(m_errorRate, m_maxDistance, m_maxAngle, positions, indices);

      RenderShaderLoader renderShaderLoader(m_singletonManager);
      return m_modelManager->addObject(db::Mesh(GL_TRIANGLES, positions, caches, indices));
    }
  }
}
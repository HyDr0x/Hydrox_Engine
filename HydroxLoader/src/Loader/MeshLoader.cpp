#include "Loader/MeshLoader.h"

#include <sstream>
#include <fstream>
#include <iostream>

#include <Utilities/Miscellaneous/SingletonManager.hpp>
#include <Utilities/PrimitiveGenerator/CubeGenerator.h>

#include <DataBase/Material.h>

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

      m_fileName = filename;

      util::ResourceHandle meshHandle;

      std::ifstream file(filename, std::ifstream::in | std::ifstream::binary);
      std::string line;

      if(file.is_open())
      {
        file.read((char*)&m_meshData, sizeof(m_meshData));

        std::vector<db::Mesh::indexType> indexData(m_meshData.indexCount);
        std::vector<GLubyte> geometryData(m_meshData.vboSize);
        std::vector<util::Cache> cacheData(m_meshData.cacheSize);
        std::vector<util::vec2ui> triangleCacheIndices(m_meshData.primitiveCount);

        file.read((char*)&indexData[0], sizeof(indexData[0]) * m_meshData.indexCount);
        file.read((char*)&geometryData[0], sizeof(geometryData[0]) * m_meshData.vboSize);
        if(m_meshData.cacheSize > 0) file.read((char*)&cacheData[0], sizeof(cacheData[0]) * m_meshData.cacheSize);
        file.read((char*)&triangleCacheIndices[0], sizeof(triangleCacheIndices[0]) * m_meshData.primitiveCount);

        meshHandle = m_modelManager->addObject(db::Mesh(db::AABB(m_meshData.bbMin, m_meshData.bbMax),
          m_meshData.primitiveType,
          m_meshData.primitiveCount,
          m_meshData.vertexCount,
          m_meshData.vertexStride,
          m_meshData.vertexDeclaration,
          geometryData, 
          cacheData, 
          triangleCacheIndices, 
          indexData));
      }
      else//wrong filename or file does not exist
      {
        file.close();

        std::clog << "Error: couldn't open mesh source file " << filename << "." << std::endl;

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
      util::CubeGenerator::generateCube(positions, indices, normals, UINT_MAX);

      std::vector<VertexElements> vertexElements;
      vertexElements.push_back(db::Mesh::MODEL_POSITION);
      vertexElements.push_back(db::Mesh::MODEL_NORMAL);

      db::Mesh mesh(GL_TRIANGLES, positions.size(), vertexElements, indices);
      mesh.copyDataIntoGeometryBuffer(db::Mesh::MODEL_POSITION, 0, positions.size(), reinterpret_cast<const GLubyte*>(&positions[0]));
      mesh.copyDataIntoGeometryBuffer(db::Mesh::MODEL_NORMAL, 0, normals.size(), reinterpret_cast<const GLubyte*>(&normals[0]));
      mesh.generateCaches(m_errorRate, m_maxDistance, m_maxAngle);

      return m_modelManager->addObject(mesh);
    }

    void MeshLoader::printFileInformations() const
    {
      std::clog << "File Informations of: " << m_fileName << std::endl;
      std::clog << "Cache Number: " << m_meshData.cacheSize << std::endl;
      std::clog << "Minimal Bounding Box: " << m_meshData.bbMin << std::endl;
      std::clog << "Maximum Bounding Box: " << m_meshData.bbMax << std::endl;
      std::clog << std::endl;
    }
  }
}
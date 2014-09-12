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
    MeshLoader::MeshLoader(util::SingletonManager *singletonManager) : ResourceLoader(singletonManager),
                                                                       m_modelManager(singletonManager->getService<db::ModelManager>())
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
      std::vector<util::Vector<float, 3>> positions;
      std::vector<db::Mesh::indexType> indices;
      util::CubeGenerator::generateCube(positions, indices);

      RenderShaderLoader renderShaderLoader(m_singletonManager);
      return m_modelManager->addObject(db::Mesh(GL_TRIANGLES, positions, indices));
    }
  }
}
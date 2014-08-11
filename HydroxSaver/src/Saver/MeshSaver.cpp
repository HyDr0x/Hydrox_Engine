#include "Saver/MeshSaver.h"

#include <sstream>
#include <fstream>

#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <DataBase/Material.h>
#include <DataBase/Texture2D.h>
#include <DataBase/RenderShader.h>

#include "Saver/RenderShaderSaver.h"
#include "Saver/ILDevilSaver.h"

namespace he
{
  namespace saver
  {
    void MeshSaver::save(std::string path, std::string filename, const util::ResourceHandle meshHandle, util::SingletonManager *singletonManager)
    {
      db::ModelManager *modelManager = singletonManager->getService<db::ModelManager>();
      db::Mesh *mesh = modelManager->getObject(meshHandle);

      std::ofstream fileStream;
      fileStream.open(path + filename + std::string(".mesh"), std::ofstream::out | std::ofstream::trunc);

      MeshMetaData meshData;
      meshData.bbMax = mesh->getBBMax();
      meshData.bbMin = mesh->getBBMin();
      meshData.primitiveType = mesh->getPrimitiveType();
      meshData.primitiveCount = mesh->getPrimitiveCount();
      meshData.indexCount = mesh->getIndexCount();
      meshData.vertexCount = mesh->getVertexCount();
      meshData.vertexDeclaration = mesh->getVertexDeclarationFlags();
      meshData.vertexStride = mesh->getVertexStride();
      meshData.vboSize = mesh->getVBOSize();

      fileStream.write((char*)&meshData, sizeof(meshData));
      fileStream.write((char*)&mesh->getIndexBuffer()[0], sizeof(mesh->getIndexBuffer()[0]) * mesh->getIndexCount());
      fileStream.write((char*)&mesh->getVBOBuffer()[0], sizeof(mesh->getVBOBuffer()[0]) * mesh->getVBOSize());
      
      fileStream.close();
    }
  }
}

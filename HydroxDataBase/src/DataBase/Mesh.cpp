#include "DataBase/Mesh.h"

#include <Utilities/Miscellaneous/CacheIndicesGenerator.h>

#include "DataBase/Material.h"

namespace he
{
  namespace db
  {
    const unsigned int Mesh::VERTEXDECLARATIONSIZE[] = {
      sizeof(util::vec3f),
      sizeof(util::vec2f),
      sizeof(util::vec2f),
      sizeof(util::vec2f),
      sizeof(util::vec2f),
      sizeof(util::vec3f),
      sizeof(util::vec3f),
      sizeof(util::vec4f),
      sizeof(util::vec4f),
      sizeof(util::vec4f),
      sizeof(util::cacheIndexType),
      sizeof(util::cacheIndexType),
      sizeof(util::vec4f),
      sizeof(util::vec4f),
      sizeof(util::vec4f),
    };

    unsigned int Mesh::vertexDeclarationSize(unsigned int index)
    {
      assert(index < VERTEXDECLARATIONFLAGNUMBER);
      return VERTEXDECLARATIONSIZE[index];
    }

    Mesh::Mesh()
    {
      m_primitiveCount = 0;
      m_vertexCount = 0;
      m_vertexStride = 0;
      m_primitiveType = GL_TRIANGLES;
    }

    Mesh::Mesh(GLenum primitiveType,
           const std::vector<util::vec3f>& positions, 
           const std::vector<util::Cache>& caches,
           const std::vector<util::vec2ui>& triangleCacheIndices,
           const std::vector<indexType>& indices,
           const std::vector<std::vector<util::vec2f>>& textureCoords, 
           const std::vector<util::vec3f>& normals, 
           const std::vector<util::vec3f>& binormals, 
           const std::vector<util::vec4f>& boneWeights,
           const std::vector<util::vec4f>& boneIndices,
           const std::vector<util::vec4f>& vertexColors) : m_boundingVolume(positions)
    {
      assert(textureCoords.size() == Material::TEXTURETYPENUM);

      m_primitiveType = primitiveType;

      unsigned int verticesPerPrimitive;
      switch(m_primitiveType)
      {
      case GL_POINTS:
        verticesPerPrimitive = 1;
        break;
      case GL_LINES:
        verticesPerPrimitive = 2;
        break;
      case GL_TRIANGLES:
      default:
        verticesPerPrimitive = 3;
        break;
      }

      m_primitiveCount = static_cast<unsigned int>(indices.size()) / verticesPerPrimitive;

      m_vertexCount = static_cast<unsigned int>(positions.size());

      m_indexData = indices;
      m_cacheData = caches;
      m_triangleCacheIndices = triangleCacheIndices;

      std::vector<he::util::cacheIndexType> cacheIndizes0;
      std::vector<he::util::cacheIndexType> cacheIndizes1;
      std::vector<util::vec3f> newNormals;

      util::CacheIndicesGenerator cacheIndicesGenerator;

      if(m_primitiveType == GL_TRIANGLES)
      {
        if(normals.empty())//generate normals if empty
        {
          generateNormals(newNormals, positions, m_indexData);
        }

        if(!m_indexData.empty())
        {
          cacheIndicesGenerator.generateCacheIndizes(positions, newNormals, m_indexData, m_cacheData, cacheIndizes0, cacheIndizes1);
        }
        else
        {
          cacheIndicesGenerator.generateCacheIndizes(positions, newNormals, m_cacheData, cacheIndizes0, cacheIndizes1);
        }
      }

      if(!normals.empty())
      {
        newNormals = normals;
      }

      m_vertexDeclaration |= positions.size() != 0 ? util::Flags<VertexDeclarationFlags>::convertToFlag(MODEL_POSITION) : util::Flags<VertexDeclarationFlags>::convertToFlag(0);
      for(unsigned int j = 0; j < textureCoords.size(); j++)
      {
        m_vertexDeclaration |= textureCoords[j].size() != 0 ? util::Flags<VertexDeclarationFlags>::convertToFlag(MODEL_TEXTURE0 + j) : util::Flags<VertexDeclarationFlags>::convertToFlag(0);
      }
      m_vertexDeclaration |= newNormals.size() != 0 ? util::Flags<VertexDeclarationFlags>::convertToFlag(MODEL_NORMAL) : util::Flags<VertexDeclarationFlags>::convertToFlag(0);
      m_vertexDeclaration |= binormals.size() != 0 ? util::Flags<VertexDeclarationFlags>::convertToFlag(MODEL_BINORMAL) : util::Flags<VertexDeclarationFlags>::convertToFlag(0);
      m_vertexDeclaration |= boneWeights.size() != 0 ? util::Flags<VertexDeclarationFlags>::convertToFlag(MODEL_BONE_WEIGHTS) : util::Flags<VertexDeclarationFlags>::convertToFlag(0);
      m_vertexDeclaration |= boneIndices.size() != 0 ? util::Flags<VertexDeclarationFlags>::convertToFlag(MODEL_BONE_INDICES) : util::Flags<VertexDeclarationFlags>::convertToFlag(0);
      m_vertexDeclaration |= vertexColors.size() != 0 ? util::Flags<VertexDeclarationFlags>::convertToFlag(MODEL_COLOR) : util::Flags<VertexDeclarationFlags>::convertToFlag(0);
      m_vertexDeclaration |= cacheIndizes0.size() != 0 ? util::Flags<VertexDeclarationFlags>::convertToFlag(MODEL_CACHEINDICES0) : util::Flags<VertexDeclarationFlags>::convertToFlag(0);
      m_vertexDeclaration |= cacheIndizes1.size() != 0 ? util::Flags<VertexDeclarationFlags>::convertToFlag(MODEL_CACHEINDICES1) : util::Flags<VertexDeclarationFlags>::convertToFlag(0);

      m_vertexStride = 0;
      GLuint strides[VERTEXDECLARATIONFLAGNUMBER];
      for(unsigned int i = 0; i < VERTEXDECLARATIONFLAGNUMBER; i++)
      {
        strides[i] = m_vertexDeclaration.toInt() & util::Flags<VertexDeclarationFlags>::convertToFlag(i).toInt() ? VERTEXDECLARATIONSIZE[i] : 0;
        m_vertexStride += strides[i];
      }
      
      m_vertexStride += m_vertexStride % 4;

      if(cacheIndicesGenerator.getNewVertexNumber() != 0)
      {
        std::vector<util::vec3f> newPositions = positions;

        const std::vector<util::VertexDataStructure>& vertices = cacheIndicesGenerator.getVertexDataStructure();
        const std::list<util::vec2ui>& newVertexEdges = cacheIndicesGenerator.getNewVertexEdgeIndices();

        unsigned int vertexIndex = 0;
        for(std::list<util::vec2ui>::const_iterator it = newVertexEdges.begin(); it != newVertexEdges.end(); it++, vertexIndex++)
        {
          unsigned int vertexIndex0 = (*it)[0], vertexIndex1 = (*it)[1];

          newPositions.push_back(vertices[vertexIndex].position);
          for(unsigned int j = 0; j < textureCoords.size(); j++)
          {
            if((m_vertexDeclaration & util::Flags<VertexDeclaration>::convertToFlag(MODEL_TEXTURE0 + j)).toInt())
            {
              textureCoords[j].push_back(util::vec2f::lerp(textureCoords[j][vertexIndex0], textureCoords[j][vertexIndex1], 0.5f));
            }
          }
          newNormals.push_back(vertices[vertexIndex].normal);
        }

        m_vertexCount = cacheIndicesGenerator.getNewVertexNumber();
      }

      GLint size = m_vertexCount * m_vertexStride;

      m_geometryData.resize(size);

      copyDataIntoGeometryBuffer(MODEL_POSITION, positions.size(), reinterpret_cast<const GLubyte*>(&positions[0]));

      for(unsigned int j = 0; j < textureCoords.size(); j++)
      {
        copyDataIntoGeometryBuffer(MODEL_TEXTURE0 + j, textureCoords[j].size(), reinterpret_cast<const GLubyte*>(&textureCoords[j][0]));
      }

      copyDataIntoGeometryBuffer(MODEL_NORMAL, newNormals.size(), reinterpret_cast<const GLubyte*>(&newNormals[0]));
      copyDataIntoGeometryBuffer(MODEL_BINORMAL, binormals.size(), reinterpret_cast<const GLubyte*>(&binormals[0]));
      copyDataIntoGeometryBuffer(MODEL_BONE_WEIGHTS, boneWeights.size(), reinterpret_cast<const GLubyte*>(&boneWeights[0]));
      copyDataIntoGeometryBuffer(MODEL_BONE_INDICES, boneIndices.size(), reinterpret_cast<const GLubyte*>(&boneIndices[0]));
      copyDataIntoGeometryBuffer(MODEL_COLOR, vertexColors.size(), reinterpret_cast<const GLubyte*>(&vertexColors[0]));
      copyDataIntoGeometryBuffer(MODEL_CACHEINDICES0, cacheIndizes0.size(), reinterpret_cast<const GLubyte*>(&cacheIndizes0[0]));
      copyDataIntoGeometryBuffer(MODEL_CACHEINDICES1, cacheIndizes1.size(), reinterpret_cast<const GLubyte*>(&cacheIndizes1[0]));

      /*for(unsigned int i = 0; i < positions.size(); i++)
      {
        std::copy((GLubyte*)&positions[i], (GLubyte*)&positions[i] + VERTEXDECLARATIONSIZE[MODEL_POSITION], &m_geometryData[0] + lokalStride + m_vertexStride * i);
      }
      lokalStride += strides[MODEL_POSITION];

      for(unsigned int j = 0; j < textureCoords.size(); j++)
      {
        for(unsigned int i = 0; i < textureCoords[j].size(); i++)
        {
          std::copy((GLubyte*)&textureCoords[j][i], (GLubyte*)&textureCoords[j][i] + VERTEXDECLARATIONSIZE[MODEL_TEXTURE0 + j], &m_geometryData[0] + lokalStride + m_vertexStride * i);
        }
        lokalStride += strides[MODEL_TEXTURE0 + j];
      }

      for(unsigned int i = 0; i < ownNormals.size(); i++)
      {
        std::copy((GLubyte*)&ownNormals[i], (GLubyte*)&ownNormals[i] + VERTEXDECLARATIONSIZE[MODEL_NORMAL], &m_geometryData[0] + lokalStride + m_vertexStride * i);
      }
      lokalStride += strides[MODEL_NORMAL];

      for(unsigned int i = 0; i < binormals.size(); i++)
      {
        std::copy((GLubyte*)&binormals[i], (GLubyte*)&binormals[i] + VERTEXDECLARATIONSIZE[MODEL_BINORMAL], &m_geometryData[0] + lokalStride + m_vertexStride * i);
      }
      lokalStride += strides[MODEL_BINORMAL];

      for(unsigned int i = 0; i < boneWeights.size(); i++)
      {
        std::copy((GLubyte*)&boneWeights[i], (GLubyte*)&boneWeights[i] + VERTEXDECLARATIONSIZE[MODEL_BONE_WEIGHTS], &m_geometryData[0] + lokalStride + m_vertexStride * i);
      }
      lokalStride += strides[MODEL_BONE_WEIGHTS];

      for(unsigned int i = 0; i < boneIndices.size(); i++)
      {
        std::copy((GLubyte*)&boneIndices[i], (GLubyte*)&boneIndices[i] + VERTEXDECLARATIONSIZE[MODEL_BONE_INDICES], &m_geometryData[0] + lokalStride + m_vertexStride * i);
      }
      lokalStride += strides[MODEL_BONE_INDICES];

      for(unsigned int i = 0; i < vertexColors.size(); i++)
      {
        std::copy((GLubyte*)&vertexColors[i], (GLubyte*)&vertexColors[i] + VERTEXDECLARATIONSIZE[MODEL_COLOR], &m_geometryData[0] + lokalStride + m_vertexStride * i);
      }
      lokalStride += strides[MODEL_COLOR];

      for(unsigned int i = 0; i < cacheIndizes0.size(); i++)
      {
        std::copy((GLubyte*)&cacheIndizes0[i], (GLubyte*)&cacheIndizes0[i] + VERTEXDECLARATIONSIZE[MODEL_CACHEINDICES0], &m_geometryData[0] + lokalStride + m_vertexStride * i);
      }
      lokalStride += strides[MODEL_CACHEINDICES0];

      for(unsigned int i = 0; i < cacheIndizes1.size(); i++)
      {
        std::copy((GLubyte*)&cacheIndizes1[i], (GLubyte*)&cacheIndizes1[i] + VERTEXDECLARATIONSIZE[MODEL_CACHEINDICES1], &m_geometryData[0] + lokalStride + m_vertexStride * i);
      }
      lokalStride += strides[MODEL_CACHEINDICES1];*/

      m_hash = MurmurHash64A(&m_geometryData[0], size, 0);
    }

    Mesh::Mesh(AABB boundingVolume,
               GLenum primitiveType,
               unsigned int primitiveCount,
               unsigned int vertexCount,
               GLuint vertexStride,
               GLuint vertexDeclarationFlags,
               const std::vector<GLubyte>& vboBuffer,
               const std::vector<util::Cache>& caches,
               const std::vector<util::vec2ui>& triangleCacheIndices,
               const std::vector<indexType>& indices
               ) :
               m_boundingVolume(boundingVolume),
               m_primitiveType(primitiveType),
               m_primitiveCount(primitiveCount),
               m_vertexCount(vertexCount),
               m_vertexStride(vertexStride),
               m_vertexDeclaration(vertexDeclarationFlags),
               m_geometryData(vboBuffer),
               m_cacheData(caches),
               m_triangleCacheIndices(triangleCacheIndices),
               m_indexData(indices)
    {
      m_hash = MurmurHash64A(&m_geometryData[0], m_vertexCount * m_vertexStride, 0);
    }

    Mesh::Mesh(const Mesh& other)
    {
      m_hash = other.m_hash;
      m_boundingVolume = other.m_boundingVolume;
      m_primitiveType = other.m_primitiveType;
      m_primitiveCount = other.m_primitiveCount;
      m_vertexCount = other.m_vertexCount;
      m_vertexStride = other.m_vertexStride;
      m_geometryData = other.m_geometryData;
      m_cacheData = other.m_cacheData;
      m_triangleCacheIndices = other.m_triangleCacheIndices;
      m_indexData = other.m_indexData;
      m_vertexDeclaration = other.m_vertexDeclaration;
    }

    Mesh::~Mesh()
    {
    }

    Mesh& Mesh::operator=(const Mesh& other)
    {
      m_hash = other.m_hash;
      m_boundingVolume = other.m_boundingVolume;
      m_primitiveType = other.m_primitiveType;
      m_primitiveCount = other.m_primitiveCount;
      m_vertexCount = other.m_vertexCount;
      m_vertexStride = other.m_vertexStride;
      m_geometryData = other.m_geometryData;
      m_cacheData = other.m_cacheData;
      m_triangleCacheIndices = other.m_triangleCacheIndices;
      m_indexData = other.m_indexData;
      m_vertexDeclaration = other.m_vertexDeclaration;

      return *this;
    }

    void Mesh::free()
    {
      m_geometryData.clear();
      m_indexData.clear();
    }

    //void Mesh::setPositions(std::vector<util::vec3f> positions)
    //{
    //  assert(m_vertexDeclaration.toInt() & util::Flags<VertexDeclarationFlags>::convertToFlag(MODEL_POSITION).toInt());

    //  for(unsigned int i = 0; i < positions.size(); i++)
    //  {
    //    std::copy((GLubyte*)&positions[i], (GLubyte*)&positions[i] + sizeof(positions[0]), &m_geometryData[0] + m_vertexStride * i);
    //  }
    //}

    //void Mesh::setTextureCoordinations(std::vector<std::vector<util::vec2f>> textureCoords)
    //{
    //  assert(textureCoords.size() == 4);

    //  GLuint strides[4];
    //  for(unsigned int i = 0; i < 4; i++)
    //  {
    //    strides[i] = m_vertexDeclaration.toInt() & util::Flags<VertexDeclarationFlags>::convertToFlag(i).toInt() ? VERTEXDECLARATIONSIZE[i] : 0;
    //  }

    //  GLuint lokalStride = 0;

    //  for(unsigned int j = 0; j < textureCoords.size(); j++)
    //  {
    //    lokalStride += strides[j];

    //    for(unsigned int i = 0; i < textureCoords[j].size(); i++)
    //    {
    //      assert(m_vertexDeclaration.toInt() & util::Flags<VertexDeclarationFlags>::convertToFlag(MODEL_TEXTURE0 + i).toInt());
    //      std::copy((GLubyte*)&textureCoords[i], (GLubyte*)&textureCoords[i] + sizeof(textureCoords[0]), &m_geometryData[0] + m_vertexStride * i + lokalStride);
    //    }
    //  }
    //}

    //void Mesh::setNormals(std::vector<util::vec3f> normals)
    //{
    //  assert(m_vertexDeclaration.toInt() & util::Flags<VertexDeclarationFlags>::convertToFlag(MODEL_NORMAL).toInt());

    //  GLuint lokalStride = 0;
    //  for(unsigned int i = 0; i < 5; i++)
    //  {
    //    lokalStride += m_vertexDeclaration.toInt() & util::Flags<VertexDeclarationFlags>::convertToFlag(i).toInt() ? VERTEXDECLARATIONSIZE[i] : 0;
    //  }

    //  for(unsigned int i = 0; i < normals.size(); i++)
    //  {
    //    std::copy((GLubyte*)&normals[i], (GLubyte*)&normals[i] + sizeof(normals[0]), &m_geometryData[0] + m_vertexStride * i + lokalStride);
    //  }
    //}

    //void Mesh::setBiNormals(std::vector<util::vec3f> binormals)
    //{
    //  assert(m_vertexDeclaration.toInt() & util::Flags<VertexDeclarationFlags>::convertToFlag(MODEL_BINORMAL).toInt());

    //  GLuint lokalStride = 0;
    //  for(unsigned int i = 0; i < 6; i++)
    //  {
    //    lokalStride += m_vertexDeclaration.toInt() & util::Flags<VertexDeclarationFlags>::convertToFlag(i).toInt() ? VERTEXDECLARATIONSIZE[i] : 0;
    //  }

    //  for(unsigned int i = 0; i < binormals.size(); i++)
    //  {
    //    std::copy((GLubyte*)&binormals[i], (GLubyte*)&binormals[i] + sizeof(binormals[0]), &m_geometryData[0] + m_vertexStride * i + lokalStride);
    //  }
    //}

    //void Mesh::setBoneWeights(std::vector<util::vec4f> boneWeights)
    //{
    //  assert(m_vertexDeclaration.toInt() & util::Flags<VertexDeclarationFlags>::convertToFlag(MODEL_BONE_WEIGHTS).toInt());

    //  GLuint lokalStride = 0;
    //  for(unsigned int i = 0; i < 7; i++)
    //  {
    //    lokalStride += m_vertexDeclaration.toInt() & util::Flags<VertexDeclarationFlags>::convertToFlag(i).toInt() ? VERTEXDECLARATIONSIZE[i] : 0;
    //  }

    //  for(unsigned int i = 0; i < boneWeights.size(); i++)
    //  {
    //    std::copy((GLubyte*)&boneWeights[i], (GLubyte*)&boneWeights[i] + sizeof(boneWeights[0]), &m_geometryData[0] + m_vertexStride * i + lokalStride);
    //  }
    //}

    //void Mesh::setBoneIndices(std::vector<util::vec4f> boneIndices)
    //{
    //  assert(m_vertexDeclaration.toInt() & util::Flags<VertexDeclarationFlags>::convertToFlag(MODEL_BONE_INDICES).toInt());

    //  GLuint lokalStride = 0;
    //  for(unsigned int i = 0; i < 8; i++)
    //  {
    //    lokalStride += m_vertexDeclaration.toInt() & util::Flags<VertexDeclarationFlags>::convertToFlag(i).toInt() ? VERTEXDECLARATIONSIZE[i] : 0;
    //  }

    //  for(unsigned int i = 0; i < boneIndices.size(); i++)
    //  {
    //    std::copy((GLubyte*)&boneIndices[i], (GLubyte*)&boneIndices[i] + sizeof(boneIndices[0]), &m_geometryData[0] + m_vertexStride * i + lokalStride);
    //  }
    //}

    //void Mesh::setVertexColors(std::vector<util::vec4f> vertexColors)
    //{
    //  assert(m_vertexDeclaration.toInt() & util::Flags<VertexDeclarationFlags>::convertToFlag(MODEL_COLOR).toInt());

    //  GLuint lokalStride = 0;
    //  for(unsigned int i = 0; i < 9; i++)
    //  {
    //    lokalStride += m_vertexDeclaration.toInt() & util::Flags<VertexDeclarationFlags>::convertToFlag(i).toInt() ? VERTEXDECLARATIONSIZE[i] : 0;
    //  }

    //  for(unsigned int i = 0; i < vertexColors.size(); i++)
    //  {
    //    std::copy((GLubyte*)&vertexColors[i], (GLubyte*)&vertexColors[i] + sizeof(vertexColors[0]), &m_geometryData[0] + m_vertexStride * i + lokalStride);
    //  }
    //}

    util::Flags<Mesh::VertexDeclaration> Mesh::getVertexDeclarationFlags() const
    {
      return m_vertexDeclaration;
    }

    GLuint Mesh::getPrimitiveType() const
    {
      return m_primitiveType;
    }

    util::vec3f Mesh::getBBMin() const
    {
      return m_boundingVolume.getBBMin();
    }

    util::vec3f Mesh::getBBMax() const
    {
      return m_boundingVolume.getBBMax();
    }

    const std::vector<GLubyte>& Mesh::getVBOBuffer() const
    {
      return m_geometryData;
    }

    GLuint Mesh::getVertexStride() const
    {
      return m_vertexStride;
    }

    GLuint Mesh::getVertexCount() const
    {
      return m_vertexCount;
    }

    GLuint Mesh::getVBOSize() const
    {
      return m_vertexCount * m_vertexStride;
    }

    const std::vector<Mesh::indexType>& Mesh::getIndexBuffer() const
    {
      return m_indexData;
    }

    GLuint Mesh::getIndexCount() const
    {
      return m_indexData.size();
    }

    GLuint Mesh::getPrimitiveCount() const
    {
      return m_primitiveCount;
    }

    GLuint Mesh::getCacheCount() const
    {
      return m_cacheData.size();
    }

    const std::vector<util::Cache>& Mesh::getCaches() const
    {
      return m_cacheData;
    }

    const std::vector<util::vec2ui>& Mesh::getTriangleCacheIndices() const
    {
      return m_triangleCacheIndices;
    }

    void Mesh::copyDataIntoGeometryBuffer(unsigned int vertexDeclaration, unsigned int numberOfElements, const GLubyte *data)
    {
      assert(m_vertexDeclaration.toInt() & util::Flags<VertexDeclaration>::convertToFlag(vertexDeclaration).toInt());

      unsigned int size = numberOfElements * VERTEXDECLARATIONSIZE[vertexDeclaration];

      GLuint localStride = 0;
      for(unsigned int i = 0; i < vertexDeclaration; i++)
      {
        localStride += m_vertexDeclaration.toInt() & util::Flags<VertexDeclarationFlags>::convertToFlag(i).toInt() ? VERTEXDECLARATIONSIZE[i] : 0;
      }

      for(unsigned int i = 0; i < size; i += VERTEXDECLARATIONSIZE[vertexDeclaration])
      {
        std::copy(&data[i], &data[i] + VERTEXDECLARATIONSIZE[vertexDeclaration], &m_geometryData[0] + localStride + m_vertexStride * i);
      }
    }

    void Mesh::generateNormals(std::vector<util::vec3f>& outNormals, const std::vector<util::vec3f>& positions, const std::vector<indexType>& indices)
    {
      outNormals.resize(positions.size());

      if(!indices.empty())
      {
        std::vector<std::list<util::vec3f>> normals(positions.size());

        for(unsigned int i = 0; i < indices.size(); i += 3)
        {
          util::vec3f v0 = positions[indices[i + 0]];
          util::vec3f v1 = positions[indices[i + 1]];
          util::vec3f v2 = positions[indices[i + 2]];

          normals[indices[i + 0]].push_back(util::math::cross(v1 - v0, v2 - v0));
          normals[indices[i + 1]].push_back(util::math::cross(v1 - v0, v2 - v0));
          normals[indices[i + 2]].push_back(util::math::cross(v1 - v0, v2 - v0));
        }

        for(unsigned int i = 0; i < normals.size(); i++)
        {
          for(std::list<util::vec3f>::iterator it = normals[i].begin(); it != normals[i].end(); it++)
          {
            outNormals[i] += *it;
          }
          outNormals[i] /= normals[i].size();
        }
      }
      else
      {
        for(unsigned int i = 0; i < positions.size(); i += 3)
        {
          util::vec3f v0 = positions[i + 0];
          util::vec3f v1 = positions[i + 1];
          util::vec3f v2 = positions[i + 2];

          outNormals[i + 0] = util::math::cross(v1 - v0, v2 - v0);
          outNormals[i + 1] = util::math::cross(v1 - v0, v2 - v0);
          outNormals[i + 2] = util::math::cross(v1 - v0, v2 - v0);
        }
      }
    }
  }
}
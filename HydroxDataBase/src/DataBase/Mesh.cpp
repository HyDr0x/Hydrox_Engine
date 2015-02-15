#include "DataBase/Mesh.h"

#include <stdarg.h>

#include <Utilities/Miscellaneous/CacheGenerator.h>
#include <Utilities/Miscellaneous/CacheIndicesGenerator.h>

#include "DataBase/Material.h"

namespace he
{
  namespace db
  {
    void sortCacheIndices(std::vector<util::vec2f>& indices, util::vec2f newValue)
    {
      std::vector<util::vec2f> sortedIndices(indices.size());

      float highestWeight = 0.0f;
      float bestIndex = FLT_MAX;

      for(unsigned int i = 0; i < indices.size(); i++)
      {
        if(newValue[1] > indices[i][1])
        {
          bestIndex = i;
          break;
        }

        sortedIndices[i] = indices[i];
      }

      if(bestIndex == FLT_MAX)
      {
        return;
      }

      sortedIndices[bestIndex] = newValue;

      for(unsigned int i = bestIndex; i < indices.size() - 1; i++)
      {
        sortedIndices[i + 1] = indices[i];
      }

      indices = sortedIndices;
    }

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
      m_vertexDeclaration = util::Flags<VertexElements>(0);
      m_primitiveCount = 0;
      m_vertexCount = 0;
      m_vertexStride = 0;
      m_primitiveType = GL_TRIANGLES;
    }

    Mesh::Mesh(GLenum primitiveType, unsigned int vertexCount, std::vector<indexType> indices, std::vector<VertexElements> flags) :
      m_vertexCount(vertexCount),
      m_indexData(indices)
    {
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

      if(m_indexData.empty())
      {
        m_primitiveCount = static_cast<unsigned int>(m_vertexCount) / verticesPerPrimitive;
      }
      else
      {
        m_primitiveCount = static_cast<unsigned int>(m_indexData.size()) / verticesPerPrimitive;
      }

      for(unsigned int i = 0; i < flags.size(); i++)
      {
        m_vertexDeclaration |= util::Flags<VertexElements>::convertToFlag(flags[i]);
      }

      m_vertexDeclaration |= util::Flags<VertexElements>::convertToFlag(MODEL_CACHEINDICES0) | util::Flags<VertexElements>::convertToFlag(MODEL_CACHEINDICES1);//we need those indices in every model

      m_vertexStride = 0;
      for(unsigned int i = 0; i < VERTEXDECLARATIONFLAGNUMBER; i++)
      {
        m_vertexStride += m_vertexDeclaration.toInt() & util::Flags<VertexElements>::convertToFlag(i).toInt() ? VERTEXDECLARATIONSIZE[i] : 0;
      }

      m_geometryData.resize(m_vertexStride * m_vertexCount);
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

    Mesh& Mesh::operator=(Mesh other)
    {
      std::swap(m_hash, other.m_hash);
      std::swap(m_boundingVolume, other.m_boundingVolume);
      std::swap(m_primitiveType, other.m_primitiveType);
      std::swap(m_primitiveCount, other.m_primitiveCount);
      std::swap(m_vertexCount, other.m_vertexCount);
      std::swap(m_vertexStride, other.m_vertexStride);
      std::swap(m_geometryData, other.m_geometryData);
      std::swap(m_cacheData, other.m_cacheData);
      std::swap(m_triangleCacheIndices, other.m_triangleCacheIndices);
      std::swap(m_indexData, other.m_indexData);
      std::swap(m_vertexDeclaration, other.m_vertexDeclaration);
      
      return *this;
    }

    void Mesh::free()
    {
      m_geometryData.clear();
      m_indexData.clear();
      m_cacheData.clear();
      m_triangleCacheIndices.clear();

      m_vertexDeclaration = util::Flags<VertexElements>(0);
      m_primitiveCount = 0;
      m_vertexCount = 0;
      m_vertexStride = 0;
      m_primitiveType = GL_TRIANGLES;
    }

    void Mesh::generateCaches(float errorRate, float maxDistance, float maxAngle, bool createCaches)
    {
      std::vector<util::vec3f> positions(m_vertexCount);
      std::vector<util::vec3f> normals(m_vertexCount);
      std::vector<he::util::cacheIndexType> cacheIndizes0;
      std::vector<he::util::cacheIndexType> cacheIndizes1;

      if(createCaches)
      {
        util::CacheGenerator generator;
        generator.initialize(errorRate, maxDistance, maxAngle);

        getDataFromGeometryBuffer(MODEL_POSITION, 0, m_vertexCount, reinterpret_cast<GLubyte*>(&positions[0]));
        getDataFromGeometryBuffer(MODEL_NORMAL, 0, m_vertexCount, reinterpret_cast<GLubyte*>(&normals[0]));

        //if(!m_indexData.empty())
        //{
        //  generator.generateCaches(m_cacheData, m_triangleCacheIndices, positions, indices);
        //}
        //else
        //{
        //  generator.generateCaches(m_cacheData, m_triangleCacheIndices, positions);
        //}

        //cacheIndizes0.resize(positions.size());
        //cacheIndizes1.resize(positions.size());

        //for(unsigned int i = 0; i < positions.size(); i++)
        //{
        //  const unsigned int CACHEINDEXSIZE = 8;

        //  std::vector<util::vec2f> cacheIndices(CACHEINDEXSIZE, util::vec2f(INT32_MAX, 0.0f));

        //  for(unsigned int j = 0; j < caches.size(); j++)
        //  {
        //    float length = (positions[i] - caches[j].position).length();
        //    if(length <= maxDistance)
        //    {
        //      float weight = util::vec3f::dot(normals[i], caches[j].normal) / (length * length + 0.0001f);//more weight is better

        //      if(weight > cacheIndices.back()[1])
        //      {
        //        sortCacheIndices(cacheIndices, util::vec2f(float(j), weight));
        //      }
        //    }
        //  }

        //  for(unsigned int j = 0; j < CACHEINDEXSIZE; j++)//convert the indices into the shader accessable format (invalid: INT32_MAX --> 0, valid: x --> x + 1)
        //  {
        //    cacheIndices[j][0] = cacheIndices[j][0] == INT32_MAX ? 0 : cacheIndices[j][0] + 1;
        //  }

        //  cacheIndizes0[i] = util::cacheIndexType(cacheIndices[0][0], cacheIndices[1][0], cacheIndices[2][0], cacheIndices[3][0]);
        //  cacheIndizes1[i] = util::cacheIndexType(cacheIndices[4][0], cacheIndices[5][0], cacheIndices[6][0], cacheIndices[7][0]);
        //}

        util::CacheIndicesGenerator cacheIndicesGenerator(maxDistance);

        if(!m_indexData.empty())
        {
          generator.generateCaches(m_cacheData, m_triangleCacheIndices, positions, m_indexData);
          cacheIndicesGenerator.generateCacheIndizes(positions, normals, m_indexData, m_cacheData, cacheIndizes0, cacheIndizes1);
        }
        else
        {
          generator.generateCaches(m_cacheData, m_triangleCacheIndices, positions);
          cacheIndicesGenerator.generateCacheIndizes(positions, normals, m_cacheData, cacheIndizes0, cacheIndizes1);
        }

        if(cacheIndicesGenerator.getNewVertexNumber() != 0)
        {
          std::vector<std::vector<util::vec2f>> textureCoordinates;
          std::vector<util::vec3f> binormals;
          std::vector<util::vec4f> boneWeights;
          std::vector<util::vec4f> boneIndices;
          std::vector<util::vec4f> vertexColors;

          for(unsigned int j = 0; j < textureCoordinates.size(); j++)
          {
            if((m_vertexDeclaration & util::Flags<VertexElements>::convertToFlag(MODEL_TEXTURE0 + j)).toInt())
            {
              getDataFromGeometryBuffer(MODEL_TEXTURE0 + j, 0, m_vertexCount, reinterpret_cast<GLubyte*>(&textureCoordinates[j][0]));
            }
          }

          if((m_vertexDeclaration & util::Flags<VertexElements>::convertToFlag(MODEL_BINORMAL)).toInt())
          {
            getDataFromGeometryBuffer(MODEL_BINORMAL, 0, m_vertexCount, reinterpret_cast<GLubyte*>(&binormals[0]));
          }

          if((m_vertexDeclaration & util::Flags<VertexElements>::convertToFlag(MODEL_BONE_WEIGHTS)).toInt())
          {
            getDataFromGeometryBuffer(MODEL_BONE_WEIGHTS, 0, m_vertexCount, reinterpret_cast<GLubyte*>(&boneWeights[0]));
          }

          if((m_vertexDeclaration & util::Flags<VertexElements>::convertToFlag(MODEL_BONE_INDICES)).toInt())
          {
            getDataFromGeometryBuffer(MODEL_BONE_INDICES, 0, m_vertexCount, reinterpret_cast<GLubyte*>(&boneIndices[0]));
          }

          if((m_vertexDeclaration & util::Flags<VertexElements>::convertToFlag(MODEL_COLOR)).toInt())
          {
            getDataFromGeometryBuffer(MODEL_COLOR, 0, m_vertexCount, reinterpret_cast<GLubyte*>(&vertexColors[0]));
          }

          const std::vector<util::VertexDataStructure>& vertices = cacheIndicesGenerator.getVertexDataStructure();
          const std::list<util::vec2ui>& newVertexEdges = cacheIndicesGenerator.getNewVertexEdgeIndices();

          unsigned int vertexIndex = m_vertexCount;
          for(std::list<util::vec2ui>::const_iterator it = newVertexEdges.begin(); it != newVertexEdges.end(); it++, vertexIndex++)
          {
            unsigned int vertexIndex0 = (*it)[0], vertexIndex1 = (*it)[1];

            positions.push_back(vertices[vertexIndex].position);

            for(unsigned int j = 0; j < textureCoordinates.size(); j++)
            {
              if((m_vertexDeclaration & util::Flags<VertexElements>::convertToFlag(MODEL_TEXTURE0 + j)).toInt())
              {
                textureCoordinates[j].push_back(util::vec2f::lerp(textureCoordinates[j][vertexIndex0], textureCoordinates[j][vertexIndex1], 0.5f));
              }
            }

            normals.push_back(vertices[vertexIndex].normal);

            if((m_vertexDeclaration & util::Flags<VertexElements>::convertToFlag(MODEL_BINORMAL)).toInt())
            {
              binormals.push_back(util::vec3f::lerp(binormals[vertexIndex0], binormals[vertexIndex1], 0.5f));
            }

            if((m_vertexDeclaration & util::Flags<VertexElements>::convertToFlag(MODEL_BONE_WEIGHTS)).toInt())
            {
              boneWeights.push_back(util::vec4f::lerp(boneWeights[vertexIndex0], boneWeights[vertexIndex1], 0.5f));
            }

            if((m_vertexDeclaration & util::Flags<VertexElements>::convertToFlag(MODEL_BONE_INDICES)).toInt())
            {
              boneIndices.push_back(util::vec4f::lerp(boneIndices[vertexIndex0], boneIndices[vertexIndex1], 0.5f));
            }

            if((m_vertexDeclaration & util::Flags<VertexElements>::convertToFlag(MODEL_COLOR)).toInt())
            {
              vertexColors.push_back(util::vec4f::lerp(vertexColors[vertexIndex0], vertexColors[vertexIndex1], 0.5f));
            }
          }

          m_vertexCount += cacheIndicesGenerator.getNewVertexNumber();

          m_geometryData.resize(m_vertexStride * m_vertexCount);

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

          m_primitiveCount = static_cast<unsigned int>(m_indexData.size()) / verticesPerPrimitive;

          copyDataIntoGeometryBuffer(MODEL_POSITION, 0, m_vertexCount, reinterpret_cast<const GLubyte*>(&positions[0]));

          for(unsigned int j = 0; j < textureCoordinates.size(); j++)
          {
            if((m_vertexDeclaration & util::Flags<VertexElements>::convertToFlag(MODEL_TEXTURE0 + j)).toInt())
            {
              copyDataIntoGeometryBuffer(MODEL_TEXTURE0 + j, 0, m_vertexCount, reinterpret_cast<const GLubyte*>(&textureCoordinates[j][0]));
            }
          }
          copyDataIntoGeometryBuffer(MODEL_NORMAL, 0, m_vertexCount, reinterpret_cast<const GLubyte*>(&normals[0]));

          if((m_vertexDeclaration & util::Flags<VertexElements>::convertToFlag(MODEL_BINORMAL)).toInt())
          {
            copyDataIntoGeometryBuffer(MODEL_BINORMAL, 0, m_vertexCount, reinterpret_cast<const GLubyte*>(&binormals[0]));
          }

          if((m_vertexDeclaration & util::Flags<VertexElements>::convertToFlag(MODEL_BONE_WEIGHTS)).toInt())
          {
            copyDataIntoGeometryBuffer(MODEL_BONE_WEIGHTS, 0, m_vertexCount, reinterpret_cast<const GLubyte*>(&boneWeights[0]));
          }

          if((m_vertexDeclaration & util::Flags<VertexElements>::convertToFlag(MODEL_BONE_INDICES)).toInt())
          {
            copyDataIntoGeometryBuffer(MODEL_BONE_INDICES, 0, m_vertexCount, reinterpret_cast<const GLubyte*>(&boneIndices[0]));
          }

          if((m_vertexDeclaration & util::Flags<VertexElements>::convertToFlag(MODEL_COLOR)).toInt())
          {
            copyDataIntoGeometryBuffer(MODEL_COLOR, 0, m_vertexCount, reinterpret_cast<const GLubyte*>(&vertexColors[0]));
          }
        }

        copyDataIntoGeometryBuffer(MODEL_CACHEINDICES0, 0, m_vertexCount, reinterpret_cast<const GLubyte*>(&cacheIndizes0[0]));
        copyDataIntoGeometryBuffer(MODEL_CACHEINDICES1, 0, m_vertexCount, reinterpret_cast<const GLubyte*>(&cacheIndizes1[0]));
      }
      else
      {
        cacheIndizes0.resize(positions.size(), util::vec4f::identity());
        cacheIndizes1.resize(positions.size(), util::vec4f::identity());
      }
    }

    void Mesh::getDataFromGeometryBuffer(unsigned int vertexDeclaration, unsigned int offset, unsigned int numberOfElements, GLubyte *data) const
    {
      assert(m_vertexDeclaration.toInt() & util::Flags<VertexElements>::convertToFlag(vertexDeclaration).toInt());

      GLuint localStride = 0;
      for(unsigned int i = 0; i < vertexDeclaration; i++)
      {
        localStride += m_vertexDeclaration.toInt() & util::Flags<VertexElements>::convertToFlag(i).toInt() ? VERTEXDECLARATIONSIZE[i] : 0;
      }

      for(unsigned int i = offset; i < numberOfElements; i++)
      {
        std::copy(&m_geometryData[i * m_vertexStride + localStride], &m_geometryData[i * m_vertexStride + localStride + VERTEXDECLARATIONSIZE[vertexDeclaration]], &data[VERTEXDECLARATIONSIZE[vertexDeclaration] * i]);
      }
    }

    void Mesh::copyDataIntoGeometryBuffer(unsigned int vertexDeclaration, unsigned int offset, unsigned int numberOfElements, const GLubyte *data)
    {
      assert(m_vertexDeclaration.toInt() & util::Flags<VertexElements>::convertToFlag(vertexDeclaration).toInt());

      GLuint localStride = 0;
      for(unsigned int i = 0; i < vertexDeclaration; i++)
      {
        localStride += m_vertexDeclaration.toInt() & util::Flags<VertexElements>::convertToFlag(i).toInt() ? VERTEXDECLARATIONSIZE[i] : 0;
      }

      for(unsigned int i = offset; i < numberOfElements; i++)
      {
        std::copy(&data[i * VERTEXDECLARATIONSIZE[vertexDeclaration]], &data[(i + 1) * VERTEXDECLARATIONSIZE[vertexDeclaration]], &m_geometryData[localStride + m_vertexStride * i]);
      }

      m_hash = MurmurHash64A(&m_geometryData[0], m_vertexCount * m_vertexStride, 0);
    }

    VertexElementFlags Mesh::getVertexDeclarationFlags() const
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
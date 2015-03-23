#include "DataBase/Mesh.h"

#include <stdarg.h>

#include <Utilities/Miscellaneous/CacheGenerator.h>
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
      m_vertexDeclaration = util::Flags<VertexElements>(0);
      m_primitiveCount = 0;
      m_vertexCount = 0;
      m_vertexStride = 0;
      m_primitiveType = GL_TRIANGLES;
    }

    Mesh::Mesh(GLenum primitiveType, unsigned int vertexCount, std::vector<VertexElements> flags, std::vector<indexType> indices) :
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

    void Mesh::generateBoundingVolume()
    {
      std::vector<util::vec3f> positions(m_vertexCount);

      getDataFromGeometryBuffer(MODEL_POSITION, 0, m_vertexCount, reinterpret_cast<GLubyte*>(&positions[0]));

      m_boundingVolume = AABB(positions);
    }

    void Mesh::generateNormals()
    {
      std::vector<util::vec3f> positions(m_vertexCount);

      getDataFromGeometryBuffer(MODEL_POSITION, 0, m_vertexCount, reinterpret_cast<GLubyte*>(&positions[0]));

      std::vector<util::vec3f> outNormals(positions.size());

      if(!m_indexData.empty())
      {
        std::vector<std::list<util::vec3f>> normals(positions.size());

        for(unsigned int i = 0; i < m_indexData.size(); i += 3)
        {
          util::vec3f v0 = positions[m_indexData[i + 0]];
          util::vec3f v1 = positions[m_indexData[i + 1]];
          util::vec3f v2 = positions[m_indexData[i + 2]];

          normals[m_indexData[i + 0]].push_back(util::math::cross(v1 - v0, v2 - v0));
          normals[m_indexData[i + 1]].push_back(util::math::cross(v1 - v0, v2 - v0));
          normals[m_indexData[i + 2]].push_back(util::math::cross(v1 - v0, v2 - v0));
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

      copyDataIntoGeometryBuffer(MODEL_NORMAL, 0, m_vertexCount, reinterpret_cast<const GLubyte*>(&outNormals[0]));
    }

    void Mesh::generateCaches(float errorRate, float maxDistance, float maxAngle)
    {
      std::vector<util::vec3f> positions(m_vertexCount);
      std::vector<util::vec3f> normals(m_vertexCount);
      std::vector<he::util::cacheIndexType> cacheIndizes0;
      std::vector<he::util::cacheIndexType> cacheIndizes1;

      util::CacheGenerator generator;
      generator.initialize(errorRate, maxDistance, maxAngle, m_boundingVolume.getBBMin(), m_boundingVolume.getBBMax());

      getDataFromGeometryBuffer(MODEL_POSITION, 0, m_vertexCount, reinterpret_cast<GLubyte*>(&positions[0]));
      getDataFromGeometryBuffer(MODEL_NORMAL, 0, m_vertexCount, reinterpret_cast<GLubyte*>(&normals[0]));

      util::CacheIndicesGenerator cacheIndicesGenerator;

      if(!m_indexData.empty())
      {
        generator.generateCaches(m_cacheData, m_triangleCacheIndices, positions, m_indexData);
      }
      else
      {
        generator.generateCaches(m_cacheData, m_triangleCacheIndices, positions);
      }

      cacheIndicesGenerator.generateCacheIndizes(positions, normals, m_cacheData, cacheIndizes0, cacheIndizes1);

      copyDataIntoGeometryBuffer(MODEL_CACHEINDICES0, 0, m_vertexCount, reinterpret_cast<const GLubyte*>(&cacheIndizes0[0]));
      copyDataIntoGeometryBuffer(MODEL_CACHEINDICES1, 0, m_vertexCount, reinterpret_cast<const GLubyte*>(&cacheIndizes1[0]));
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
  }
}
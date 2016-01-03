#include "DataBase/Mesh.h"

#include <stdarg.h>

#include <Utilities/Miscellaneous/ISMOcclusionDiscGenerator.h>

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
               const std::vector<util::vec4f>& occluder,
               const std::vector<util::vec2ui>& triangleOccluderIndices,
               const std::vector<indexType>& indices
               ) :
               m_boundingVolume(boundingVolume),
               m_primitiveType(primitiveType),
               m_primitiveCount(primitiveCount),
               m_vertexCount(vertexCount),
               m_vertexStride(vertexStride),
               m_vertexDeclaration(vertexDeclarationFlags),
               m_geometryData(vboBuffer),
               m_occluderCoordinates(occluder),
               m_triangleOccluderIndices(triangleOccluderIndices),
               m_indexData(indices)
    {
    }

    Mesh::Mesh(const Mesh& other) : ManagedResource(other)
    {
      m_boundingVolume = other.m_boundingVolume;
      m_primitiveType = other.m_primitiveType;
      m_primitiveCount = other.m_primitiveCount;
      m_vertexCount = other.m_vertexCount;
      m_vertexStride = other.m_vertexStride;
      m_geometryData = other.m_geometryData;
      m_occluderCoordinates = other.m_occluderCoordinates;
      m_triangleOccluderIndices = other.m_triangleOccluderIndices;
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
      std::swap(m_occluderCoordinates, other.m_occluderCoordinates);
      std::swap(m_triangleOccluderIndices, other.m_triangleOccluderIndices);
      std::swap(m_indexData, other.m_indexData);
      std::swap(m_vertexDeclaration, other.m_vertexDeclaration);
      
      return *this;
    }

    void Mesh::free()
    {
      m_geometryData.clear();
      m_indexData.clear();

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
      if(m_primitiveType == GL_TRIANGLES)
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
    }

    void Mesh::generateISMOccluderPoints(float errorRate, float maxDistance, float maxAngle)
    {
      if(m_primitiveType == GL_TRIANGLES)
      {
        std::vector<util::vec3f> positions(m_vertexCount);

        util::ISMOcclusionDiscGenerator generator;
        generator.initialize(errorRate, maxDistance, maxAngle, m_boundingVolume.getBBMin(), m_boundingVolume.getBBMax());

        getDataFromGeometryBuffer(MODEL_POSITION, 0, m_vertexCount, reinterpret_cast<GLubyte*>(&positions[0]));

        if(!m_indexData.empty())
        {
          generator.generateOccluder(m_occluderCoordinates, m_triangleOccluderIndices, positions, m_indexData);
        }
        else
        {
          generator.generateOccluder(m_occluderCoordinates, m_triangleOccluderIndices, positions);
        }
      }
      /*
      if(m_primitiveType == GL_TRIANGLES)
      {
        std::vector<util::vec3f> positions(m_vertexCount);
        std::vector<util::vec3f> normals(m_vertexCount);
        getDataFromGeometryBuffer(MODEL_POSITION, 0, m_vertexCount, reinterpret_cast<GLubyte*>(&positions[0]));

        std::vector<util::vec3f> trianglePositions;
        if(m_indexData.empty())
        {
          trianglePositions = positions;
        }
        else
        {
          trianglePositions.resize(m_indexData.size());
          for(unsigned int i = 0; i < trianglePositions.size(); i++)
          {
            trianglePositions[i] = positions[m_indexData[i]];
          }
        }

        //calculate the overall area of the mesh triangles
        float triangleArea = 0.0f;
        for(unsigned int i = 0; i < trianglePositions.size(); i += 3)
        {
          util::vec3f a = trianglePositions[i];
          util::vec3f b = trianglePositions[i + 1];
          util::vec3f c = trianglePositions[i + 2];

          util::vec3f ab = b - a;
          util::vec3f ac = c - a;

          triangleArea += 0.5f * util::math::cross(ab, ac).length();

          unsigned int temporaryOccluderNumber = triangleArea * pointsPerArea;
          if(temporaryOccluderNumber > 0)
          {
            triangleArea = 0.0f;

            for(unsigned j = 0; j < temporaryOccluderNumber; j++)
            {
              float alpha = float(rand()) / float(RAND_MAX);
              float beta = (1.0f - alpha) * float(rand()) / float(RAND_MAX);
              float gamma = 1.0f - alpha - beta;
              m_occluderCoordinates.push_back(util::vec4f(alpha, beta, gamma, 0.0f));//barycentric coordinates
            }
          }

          m_triangleOccluderIndices.push_back(util::vec2ui(m_occluderCoordinates.size() - temporaryOccluderNumber, m_occluderCoordinates.size()));
        }
      }*/
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
        std::copy(m_geometryData.begin() + (i * m_vertexStride + localStride), m_geometryData.begin() + (i * m_vertexStride + localStride + VERTEXDECLARATIONSIZE[vertexDeclaration]), &data[VERTEXDECLARATIONSIZE[vertexDeclaration] * i]);
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
        std::copy(&data[i * VERTEXDECLARATIONSIZE[vertexDeclaration]], &data[(i + 1) * VERTEXDECLARATIONSIZE[vertexDeclaration]], m_geometryData.begin() + (localStride + m_vertexStride * i));
      }

      m_dirtyHash = true;
    }

    VertexElementFlags Mesh::getVertexDeclarationFlags() const
    {
      return m_vertexDeclaration;
    }

    GLuint Mesh::getPrimitiveType() const
    {
      return m_primitiveType;
    }

    const AABB& Mesh::getBoundingVolume() const
    {
      return m_boundingVolume;
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

    GLuint Mesh::getOccluderCount() const
    {
      return m_occluderCoordinates.size();
    }

    const std::vector<util::vec4f>& Mesh::getOccluder() const
    {
      return m_occluderCoordinates;
    }
    const std::vector<util::vec2ui>& Mesh::getTriangleOccluderIndices() const
    {
      return m_triangleOccluderIndices;
    }

    void Mesh::updateHash()
    {
      std::vector<GLubyte> hashData(m_vertexCount * m_vertexStride + sizeof(util::vec4f) * m_occluderCoordinates.size());

      std::copy(m_geometryData.begin(), m_geometryData.begin() + m_geometryData.size(), &hashData[0]);

      if(!m_occluderCoordinates.empty())
      {
        GLubyte *occluderPointer = reinterpret_cast<GLubyte*>(&m_occluderCoordinates[0]);
        std::copy(occluderPointer, &occluderPointer[m_occluderCoordinates.size() * sizeof(util::vec4f)], &hashData[m_vertexCount * m_vertexStride]);
      }
      
      m_hash = MurmurHash64A(&hashData[0], hashData.size(), 0);
    }
  }
}
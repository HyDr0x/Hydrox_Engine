#include "DataBase/Mesh.h"

namespace he
{
  namespace db
  {
    const unsigned int Mesh::VERTEXDECLARATIONFLAGS[] = {
      1,
      2,
      4,
      8,
      16,
      32,
      64,
      128,
      256,
      512,
      //1024,
      //2048,
      4096,
      8192,
      16384,
    };

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
      //sizeof(util::PointCloudGenerator::cacheIndexType),
      //sizeof(util::PointCloudGenerator::cacheIndexType),
      sizeof(util::vec4f),
      sizeof(util::vec4f),
      sizeof(util::vec4f),
    };

    unsigned int Mesh::vertexDeclarationFlag(unsigned int index)
    {
      assert(index < VERTEXDECLARATIONFLAGNUMBER);
      return VERTEXDECLARATIONFLAGS[index];
    }

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
      m_vertexDeclarationFlags = 0;
      m_primitiveType = GL_TRIANGLES;
    }

    Mesh::Mesh(GLenum primitiveType,
           const std::vector<util::vec3f>& positions, 
           const std::vector<util::PointCloudGenerator::cacheIndexType>& cacheIndizes0,
           const std::vector<util::PointCloudGenerator::cacheIndexType>& cacheIndizes1,
           const std::vector<indexType>& indices,
           const std::vector<std::vector<util::vec2f>>& textureCoords, 
           const std::vector<util::vec3f>& normals, 
           const std::vector<util::vec3f>& binormals, 
           const std::vector<util::vec4f>& boneWeights,
           const std::vector<util::vec4f>& boneIndices,
           const std::vector<util::vec4f>& vertexColors) : m_boundingVolume(positions)
    {
      assert(textureCoords.size() == 4);

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

      m_vertexDeclarationFlags = 0;

      m_vertexDeclarationFlags |= positions.size() != 0 ? VERTEXDECLARATIONFLAGS[MODEL_POSITION] : 0;
      m_vertexDeclarationFlags |= textureCoords[0].size() != 0 ? VERTEXDECLARATIONFLAGS[MODEL_TEXTURE0] : 0;
      m_vertexDeclarationFlags |= textureCoords[1].size() != 0 ? VERTEXDECLARATIONFLAGS[MODEL_TEXTURE1] : 0;
      m_vertexDeclarationFlags |= textureCoords[2].size() != 0 ? VERTEXDECLARATIONFLAGS[MODEL_TEXTURE2] : 0;
      m_vertexDeclarationFlags |= textureCoords[3].size() != 0 ? VERTEXDECLARATIONFLAGS[MODEL_TEXTURE3] : 0;
      m_vertexDeclarationFlags |= normals.size() != 0 ? VERTEXDECLARATIONFLAGS[MODEL_NORMAL] : 0;
      m_vertexDeclarationFlags |= binormals.size() != 0 ? VERTEXDECLARATIONFLAGS[MODEL_BINORMAL] : 0;
      m_vertexDeclarationFlags |= boneWeights.size() != 0 ? VERTEXDECLARATIONFLAGS[MODEL_BONE_WEIGHTS] : 0;
      m_vertexDeclarationFlags |= boneIndices.size() != 0 ? VERTEXDECLARATIONFLAGS[MODEL_BONE_INDICES] : 0;
      m_vertexDeclarationFlags |= vertexColors.size() != 0 ? VERTEXDECLARATIONFLAGS[MODEL_COLOR] : 0;
      //m_vertexDeclarationFlags |= cacheIndizes0.size() != 0 ? VERTEXDECLARATIONFLAGS[MODEL_CACHEINDIZES0] : 0;
      //m_vertexDeclarationFlags |= cacheIndizes1.size() != 0 ? VERTEXDECLARATIONFLAGS[MODEL_CACHEINDIZES1] : 0;

      m_vertexStride = 0;
      GLuint strides[VERTEXDECLARATIONFLAGNUMBER];
      for(unsigned int i = 0; i < VERTEXDECLARATIONFLAGNUMBER; i++)
      {
        strides[i] = m_vertexDeclarationFlags & VERTEXDECLARATIONFLAGS[i] ? VERTEXDECLARATIONSIZE[i] : 0;
        m_vertexStride += strides[i];
      }
      
      m_vertexStride += m_vertexStride % 4;

      GLint size = m_vertexCount * m_vertexStride;
      GLuint lokalStride = 0;

      m_geometryData.resize(size);

      for(unsigned int i = 0; i < positions.size(); i++)
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

      for(unsigned int i = 0; i < normals.size(); i++)
      {
        std::copy((GLubyte*)&normals[i], (GLubyte*)&normals[i] + VERTEXDECLARATIONSIZE[MODEL_NORMAL], &m_geometryData[0] + lokalStride + m_vertexStride * i);
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

      //for(unsigned int i = 0; i < cacheIndizes0.size(); i++)
      //{
      //  std::copy((GLubyte*)&cacheIndizes0[i], (GLubyte*)&cacheIndizes0[i] + VERTEXDECLARATIONSIZE[MODEL_CACHEINDIZES0], &m_geometryData[0] + lokalStride + m_vertexStride * i);
      //}
      //lokalStride += strides[MODEL_CACHEINDIZES0];

      //for(unsigned int i = 0; i < cacheIndizes1.size(); i++)
      //{
      //  std::copy((GLubyte*)&cacheIndizes1[i], (GLubyte*)&cacheIndizes1[i] + VERTEXDECLARATIONSIZE[MODEL_CACHEINDIZES1], &m_geometryData[0] + lokalStride + m_vertexStride * i);
      //}
      //lokalStride += strides[MODEL_CACHEINDIZES1];

      m_hash = MurmurHash64A(&m_geometryData[0], size, 0);

      m_indexData = indices;

      //const GLenum ErrorValue = glGetError();
      //int tmpE = 0;
      //if(ErrorValue != GL_NO_ERROR)
      //  tmpE++;
      //tmpE = GL_INVALID_VALUE & GL_INVALID_VALUE;
    }

    Mesh::Mesh(AABB boundingVolume,
               GLenum primitiveType,
               unsigned int primitiveCount,
               unsigned int vertexCount,
               GLuint vertexStride,
               GLuint vertexDeclarationFlags,
               const std::vector<GLubyte>& vboBuffer, 
               const std::vector<indexType>& indices
               ) :
               m_boundingVolume(boundingVolume),
               m_primitiveType(primitiveType),
               m_primitiveCount(primitiveCount),
               m_vertexCount(vertexCount),
               m_vertexStride(vertexStride),
               m_vertexDeclarationFlags(vertexDeclarationFlags),
               m_geometryData(vboBuffer), 
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
      m_indexData = other.m_indexData;
      m_vertexDeclarationFlags = other.m_vertexDeclarationFlags;
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
      m_indexData = other.m_indexData;
      m_vertexDeclarationFlags = other.m_vertexDeclarationFlags;

      return *this;
    }

    void Mesh::free()
    {
      m_geometryData.clear();
      m_indexData.clear();
    }

    void Mesh::setPositions(std::vector<util::vec3f> positions)
    {
      assert(m_vertexDeclarationFlags & VERTEXDECLARATIONFLAGS[MODEL_POSITION]);

      for(unsigned int i = 0; i < positions.size(); i++)
      {
        std::copy((GLubyte*)&positions[i], (GLubyte*)&positions[i] + sizeof(positions[0]), &m_geometryData[0] + m_vertexStride * i);
      }
    }

    void Mesh::setTextureCoordinations(std::vector<std::vector<util::vec2f>> textureCoords)
    {
      assert(textureCoords.size() == 4);

      GLuint strides[5];
      for(unsigned int i = 0; i < 5; i++)
      {
        strides[i] = m_vertexDeclarationFlags & VERTEXDECLARATIONFLAGS[i] ? VERTEXDECLARATIONSIZE[i] : 0;
      }

      GLuint lokalStride = 0;

      for(unsigned int j = 0; j < textureCoords.size(); j++)
      {
        lokalStride += strides[j];

        for(unsigned int i = 0; i < textureCoords[j].size(); i++)
        {
          assert(m_vertexDeclarationFlags & VERTEXDECLARATIONFLAGS[MODEL_TEXTURE0 + i]);
          std::copy((GLubyte*)&textureCoords[i], (GLubyte*)&textureCoords[i] + sizeof(textureCoords[0]), &m_geometryData[0] + m_vertexStride * i + lokalStride);
        }
      }
    }

    void Mesh::setNormals(std::vector<util::vec3f> normals)
    {
      assert(m_vertexDeclarationFlags & VERTEXDECLARATIONFLAGS[MODEL_NORMAL]);

      GLuint lokalStride = 0;
      for(unsigned int i = 0; i < 5; i++)
      {
        lokalStride += m_vertexDeclarationFlags & VERTEXDECLARATIONFLAGS[i] ? VERTEXDECLARATIONSIZE[i] : 0;
      }

      for(unsigned int i = 0; i < normals.size(); i++)
      {
        std::copy((GLubyte*)&normals[i], (GLubyte*)&normals[i] + sizeof(normals[0]), &m_geometryData[0] + m_vertexStride * i + lokalStride);
      }
    }

    void Mesh::setBiNormals(std::vector<util::vec3f> binormals)
    {
      assert(m_vertexDeclarationFlags & VERTEXDECLARATIONFLAGS[MODEL_BINORMAL]);

      GLuint lokalStride = 0;
      for(unsigned int i = 0; i < 6; i++)
      {
        lokalStride += m_vertexDeclarationFlags & VERTEXDECLARATIONFLAGS[i] ? VERTEXDECLARATIONSIZE[i] : 0;
      }

      for(unsigned int i = 0; i < binormals.size(); i++)
      {
        std::copy((GLubyte*)&binormals[i], (GLubyte*)&binormals[i] + sizeof(binormals[0]), &m_geometryData[0] + m_vertexStride * i + lokalStride);
      }
    }

    void Mesh::setBoneWeights(std::vector<util::vec4f> boneWeights)
    {
      assert(m_vertexDeclarationFlags & VERTEXDECLARATIONFLAGS[MODEL_BONE_WEIGHTS]);

      GLuint lokalStride = 0;
      for(unsigned int i = 0; i < 7; i++)
      {
        lokalStride += m_vertexDeclarationFlags & VERTEXDECLARATIONFLAGS[i] ? VERTEXDECLARATIONSIZE[i] : 0;
      }

      for(unsigned int i = 0; i < boneWeights.size(); i++)
      {
        std::copy((GLubyte*)&boneWeights[i], (GLubyte*)&boneWeights[i] + sizeof(boneWeights[0]), &m_geometryData[0] + m_vertexStride * i + lokalStride);
      }
    }

    void Mesh::setBoneIndices(std::vector<util::vec4f> boneIndices)
    {
      assert(m_vertexDeclarationFlags & VERTEXDECLARATIONFLAGS[MODEL_BONE_INDICES]);

      GLuint lokalStride = 0;
      for(unsigned int i = 0; i < 8; i++)
      {
        lokalStride += m_vertexDeclarationFlags & VERTEXDECLARATIONFLAGS[i] ? VERTEXDECLARATIONSIZE[i] : 0;
      }

      for(unsigned int i = 0; i < boneIndices.size(); i++)
      {
        std::copy((GLubyte*)&boneIndices[i], (GLubyte*)&boneIndices[i] + sizeof(boneIndices[0]), &m_geometryData[0] + m_vertexStride * i + lokalStride);
      }
    }

    void Mesh::setVertexColors(std::vector<util::vec4f> vertexColors)
    {
      assert(m_vertexDeclarationFlags & VERTEXDECLARATIONFLAGS[MODEL_COLOR]);

      GLuint lokalStride = 0;
      for(unsigned int i = 0; i < 9; i++)
      {
        lokalStride += m_vertexDeclarationFlags & VERTEXDECLARATIONFLAGS[i] ? VERTEXDECLARATIONSIZE[i] : 0;
      }

      for(unsigned int i = 0; i < vertexColors.size(); i++)
      {
        std::copy((GLubyte*)&vertexColors[i], (GLubyte*)&vertexColors[i] + sizeof(vertexColors[0]), &m_geometryData[0] + m_vertexStride * i + lokalStride);
      }
    }

    GLuint Mesh::getVertexDeclarationFlags() const
    {
      return m_vertexDeclarationFlags;
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
  }
}
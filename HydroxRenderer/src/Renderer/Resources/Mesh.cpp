#include "Renderer/Resources/Mesh.h"

namespace he
{
	namespace renderer
	{
    Mesh::Mesh()
    {
      m_primitiveCount = 0;
      m_vertexCount = 0;
      m_vertexStride = 0;
      m_vertexDeclarationFlags = 0;
      m_primitiveType = GL_TRIANGLES;
    }

    Mesh::Mesh(GLenum primitiveType,
           const std::vector<util::Vector<float, 3>>& positions, 
           const std::vector<indexType>& indices,
           const std::vector<std::vector<util::Vector<float, 2>>>& textureCoords, 
           const std::vector<util::Vector<float, 3>>& normals, 
           const std::vector<util::Vector<float, 3>>& binormals, 
           const std::vector<util::Vector<float, 4>>& boneWeights,
           const std::vector<util::Vector<float, 4>>& boneIndices,
           const std::vector<util::Vector<float, 4>>& vertexColors) : m_boundingVolume(positions)
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

      m_vertexDeclarationFlags |= positions.size()        != 0 ? renderer::Mesh::MODEL_POSITION     : 0;
      m_vertexDeclarationFlags |= textureCoords[0].size() != 0 ? renderer::Mesh::MODEL_TEXTURE0     : 0;
      m_vertexDeclarationFlags |= textureCoords[1].size() != 0 ? renderer::Mesh::MODEL_TEXTURE1     : 0;
      m_vertexDeclarationFlags |= textureCoords[2].size() != 0 ? renderer::Mesh::MODEL_TEXTURE2     : 0;
      m_vertexDeclarationFlags |= textureCoords[3].size() != 0 ? renderer::Mesh::MODEL_TEXTURE3     : 0;
      m_vertexDeclarationFlags |= normals.size()          != 0 ? renderer::Mesh::MODEL_NORMAL       : 0;
      m_vertexDeclarationFlags |= binormals.size()        != 0 ? renderer::Mesh::MODEL_BINORMAL     : 0;
      m_vertexDeclarationFlags |= boneWeights.size()      != 0 ? renderer::Mesh::MODEL_BONE_WEIGHTS : 0;
      m_vertexDeclarationFlags |= boneIndices.size()      != 0 ? renderer::Mesh::MODEL_BONE_INDICES : 0;
      m_vertexDeclarationFlags |= vertexColors.size()     != 0 ? renderer::Mesh::MODEL_COLOR        : 0;

      GLuint texStride[4];

      GLuint posStride          = m_vertexDeclarationFlags & MODEL_POSITION     ? sizeof(positions[0]) : 0;
      texStride[0]              = m_vertexDeclarationFlags & MODEL_TEXTURE0     ? sizeof(textureCoords[0][0]) : 0;
      texStride[1]              = m_vertexDeclarationFlags & MODEL_TEXTURE1     ? sizeof(textureCoords[1][0]) : 0;
      texStride[2]              = m_vertexDeclarationFlags & MODEL_TEXTURE2     ? sizeof(textureCoords[2][0]) : 0;
      texStride[3]              = m_vertexDeclarationFlags & MODEL_TEXTURE3     ? sizeof(textureCoords[3][0]) : 0;
      GLuint normalStride       = m_vertexDeclarationFlags & MODEL_NORMAL       ? sizeof(normals[0]) : 0;
      GLuint binormalStride     = m_vertexDeclarationFlags & MODEL_BINORMAL     ? sizeof(binormals[0]) : 0;
      GLuint boneWeightStride   = m_vertexDeclarationFlags & MODEL_BONE_WEIGHTS ? sizeof(boneWeights[0]) : 0;
      GLuint boneIndexStride    = m_vertexDeclarationFlags & MODEL_BONE_INDICES ? sizeof(boneIndices[0]) : 0;
      GLuint vertexColorStride  = m_vertexDeclarationFlags & MODEL_COLOR        ? sizeof(vertexColors[0]) : 0;

      m_vertexStride = posStride + texStride[0] + texStride[1] + texStride[2] + texStride[3] + normalStride + binormalStride + boneWeightStride + boneIndexStride + vertexColorStride;

      m_vertexStride += m_vertexStride % 4;

      GLint size = m_vertexCount * m_vertexStride;
      GLuint lokalStride = 0;

      m_geometryData.resize(size);

      for(unsigned int i = 0; i < positions.size(); i++)
		  {
        std::copy((GLubyte*)&positions[i], (GLubyte*)&positions[i] + sizeof(positions[0]), &m_geometryData[0] + lokalStride + m_vertexStride * i);
      }
      lokalStride += posStride;

      for(unsigned int j = 0; j < textureCoords.size(); j++)
      {
        for(unsigned int i = 0; i < textureCoords[j].size(); i++)
		    {
          std::copy((GLubyte*)&textureCoords[j][i], (GLubyte*)&textureCoords[j][i] + sizeof(textureCoords[0][0]), &m_geometryData[0] + lokalStride + m_vertexStride * i);
        }
        lokalStride += texStride[j];
      }

      for(unsigned int i = 0; i < normals.size(); i++)
		  {
        std::copy((GLubyte*)&normals[i], (GLubyte*)&normals[i] + sizeof(normals[0]), &m_geometryData[0] + lokalStride + m_vertexStride * i);
      }
      lokalStride += normalStride;

      for(unsigned int i = 0; i < binormals.size(); i++)
		  {
        std::copy((GLubyte*)&binormals[i], (GLubyte*)&binormals[i] + sizeof(binormals[0]), &m_geometryData[0] + lokalStride + m_vertexStride * i);
      }
      lokalStride += binormalStride;

      for(unsigned int i = 0; i < boneWeights.size(); i++)
		  {
        std::copy((GLubyte*)&boneWeights[i], (GLubyte*)&boneWeights[i] + sizeof(boneWeights[0]), &m_geometryData[0] + lokalStride + m_vertexStride * i);
      }
      lokalStride += boneWeightStride;

      for(unsigned int i = 0; i < boneIndices.size(); i++)
		  {
        std::copy((GLubyte*)&boneIndices[i], (GLubyte*)&boneIndices[i] + sizeof(boneIndices[0]), &m_geometryData[0] + lokalStride + m_vertexStride * i);
      }
      lokalStride += boneIndexStride;

      for(unsigned int i = 0; i < vertexColors.size(); i++)
		  {
        std::copy((GLubyte*)&vertexColors[i], (GLubyte*)&vertexColors[i] + sizeof(vertexColors[0]), &m_geometryData[0] + lokalStride + m_vertexStride * i);
      }
      lokalStride += vertexColorStride;

      m_hash = MurmurHash64A(&m_geometryData[0], size, 0);

      m_indexData = indices;

	    //const GLenum ErrorValue = glGetError();
	    //int tmpE = 0;
	    //if(ErrorValue != GL_NO_ERROR)
	    //	tmpE++;
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

    Mesh::Mesh(const Mesh& o)
    {
      m_hash = o.m_hash;
      m_boundingVolume = o.m_boundingVolume;
      m_primitiveType = o.m_primitiveType;
	    m_primitiveCount = o.m_primitiveCount;
      m_vertexCount = o.m_vertexCount;
      m_vertexStride = o.m_vertexStride;
      m_geometryData = o.m_geometryData;
      m_indexData = o.m_indexData;
      m_vertexDeclarationFlags = o.m_vertexDeclarationFlags;
    }

    Mesh::~Mesh()
    {
    }

    Mesh& Mesh::operator=(const Mesh& o)
    {
      m_hash = o.m_hash;
      m_boundingVolume = o.m_boundingVolume;
      m_primitiveType = o.m_primitiveType;
	    m_primitiveCount = o.m_primitiveCount;
      m_vertexCount = o.m_vertexCount;
      m_vertexStride = o.m_vertexStride;
      m_geometryData = o.m_geometryData;
      m_indexData = o.m_indexData;
      m_vertexDeclarationFlags = o.m_vertexDeclarationFlags;

      return *this;
    }

    void Mesh::free()
    {
      m_geometryData.clear();
      m_indexData.clear();
    }

    void Mesh::setPositions(std::vector<util::Vector<float, 3>> positions)
    {
      assert(m_vertexDeclarationFlags & MODEL_POSITION);

      for(unsigned int i = 0; i < positions.size(); i++)
		  {
        std::copy((GLubyte*)&positions[i], (GLubyte*)&positions[i] + sizeof(positions[0]), &m_geometryData[0] + m_vertexStride * i);
      }
    }

    void Mesh::setTextureCoordinations(std::vector<std::vector<util::Vector<float, 2>>> textureCoords)
    {
      assert(textureCoords.size() == 4 && m_vertexDeclarationFlags & MODEL_TEXTURE0);

      GLuint texStride[4];

      GLuint posStride = m_vertexDeclarationFlags & MODEL_POSITION ? sizeof(util::Vector<float, 3>) : 0;
      texStride[0] = m_vertexDeclarationFlags & MODEL_TEXTURE0 ? sizeof(util::Vector<float, 2>) : 0;
      texStride[1] = m_vertexDeclarationFlags & MODEL_TEXTURE1 ? sizeof(util::Vector<float, 2>) : 0;
      texStride[2] = m_vertexDeclarationFlags & MODEL_TEXTURE2 ? sizeof(util::Vector<float, 2>) : 0;
      texStride[3] = m_vertexDeclarationFlags & MODEL_TEXTURE3 ? sizeof(util::Vector<float, 2>) : 0;

      GLuint lokalStride = posStride;

      for(unsigned int j = 0; j < textureCoords.size(); j++)
      {
        for(unsigned int i = 0; i < textureCoords[j].size(); i++)
		    {
          std::copy((GLubyte*)&textureCoords[i], (GLubyte*)&textureCoords[i] + sizeof(textureCoords[0]), &m_geometryData[0] + m_vertexStride * i);
        }
        lokalStride += texStride[j];
      }
    }

    void Mesh::setNormals(std::vector<util::Vector<float, 3>> normals)
    {
      assert(m_vertexDeclarationFlags & MODEL_NORMAL);

      GLuint texStride[4];

      GLuint posStride = m_vertexDeclarationFlags & MODEL_POSITION ? sizeof(util::Vector<float, 3>) : 0;
      texStride[0] = m_vertexDeclarationFlags & MODEL_TEXTURE0 ? sizeof(util::Vector<float, 2>) : 0;
      texStride[1] = m_vertexDeclarationFlags & MODEL_TEXTURE1 ? sizeof(util::Vector<float, 2>) : 0;
      texStride[2] = m_vertexDeclarationFlags & MODEL_TEXTURE2 ? sizeof(util::Vector<float, 2>) : 0;
      texStride[3] = m_vertexDeclarationFlags & MODEL_TEXTURE3 ? sizeof(util::Vector<float, 2>) : 0;

      GLuint lokalStride = posStride + texStride[0] + texStride[1] + texStride[2] + texStride[3];

      for(unsigned int i = 0; i < normals.size(); i++)
		  {
        std::copy((GLubyte*)&normals[i], (GLubyte*)&normals[i] + sizeof(normals[0]), &m_geometryData[0] + m_vertexStride * i);
      }
    }

    void Mesh::setBiNormals(std::vector<util::Vector<float, 3>> binormals)
    {
      assert(m_vertexDeclarationFlags & MODEL_BINORMAL);

      GLuint texStride[4];

      GLuint posStride = m_vertexDeclarationFlags & MODEL_POSITION ? sizeof(util::Vector<float, 3>) : 0;
      texStride[0] = m_vertexDeclarationFlags & MODEL_TEXTURE0 ? sizeof(util::Vector<float, 2>) : 0;
      texStride[1] = m_vertexDeclarationFlags & MODEL_TEXTURE1 ? sizeof(util::Vector<float, 2>) : 0;
      texStride[2] = m_vertexDeclarationFlags & MODEL_TEXTURE2 ? sizeof(util::Vector<float, 2>) : 0;
      texStride[3] = m_vertexDeclarationFlags & MODEL_TEXTURE3 ? sizeof(util::Vector<float, 2>) : 0;
      GLuint normalStride  = m_vertexDeclarationFlags & MODEL_NORMAL   ? sizeof(util::Vector<float, 3>) : 0;

      GLuint lokalStride = posStride + texStride[0] + texStride[1] + texStride[2] + texStride[3] + normalStride;

      for(unsigned int i = 0; i < binormals.size(); i++)
		  {
        std::copy((GLubyte*)&binormals[i], (GLubyte*)&binormals[i] + sizeof(binormals[0]), &m_geometryData[0] + m_vertexStride * i);
      }
    }

    void Mesh::setBoneWeights(std::vector<util::Vector<float, 4>> boneWeights)
    {
      assert(m_vertexDeclarationFlags & MODEL_BONE_WEIGHTS);

      GLuint texStride[4];

      GLuint posStride      = m_vertexDeclarationFlags & MODEL_POSITION ? sizeof(util::Vector<float, 3>) : 0;
      texStride[0] = m_vertexDeclarationFlags & MODEL_TEXTURE0 ? sizeof(util::Vector<float, 2>) : 0;
      texStride[1] = m_vertexDeclarationFlags & MODEL_TEXTURE1 ? sizeof(util::Vector<float, 2>) : 0;
      texStride[2] = m_vertexDeclarationFlags & MODEL_TEXTURE2 ? sizeof(util::Vector<float, 2>) : 0;
      texStride[3] = m_vertexDeclarationFlags & MODEL_TEXTURE3 ? sizeof(util::Vector<float, 2>) : 0;
      GLuint normalStride   = m_vertexDeclarationFlags & MODEL_NORMAL   ? sizeof(util::Vector<float, 3>) : 0;
      GLuint binormalStride = m_vertexDeclarationFlags & MODEL_BINORMAL ? sizeof(util::Vector<float, 3>) : 0;

      GLuint lokalStride = posStride + texStride[0] + texStride[1] + texStride[2] + texStride[3] + normalStride + binormalStride;

      for(unsigned int i = 0; i < boneWeights.size(); i++)
		  {
        std::copy((GLubyte*)&boneWeights[i], (GLubyte*)&boneWeights[i] + sizeof(boneWeights[0]), &m_geometryData[0] + m_vertexStride * i);
      }
    }

    void Mesh::setBoneIndices(std::vector<util::Vector<float, 4>> boneIndices)
    {
      assert(m_vertexDeclarationFlags & MODEL_BONE_INDICES);

      GLuint texStride[4];

      GLuint posStride        = m_vertexDeclarationFlags & MODEL_POSITION      ? sizeof(util::Vector<float, 3>) : 0;
      texStride[0] = m_vertexDeclarationFlags & MODEL_TEXTURE0 ? sizeof(util::Vector<float, 2>) : 0;
      texStride[1] = m_vertexDeclarationFlags & MODEL_TEXTURE1 ? sizeof(util::Vector<float, 2>) : 0;
      texStride[2] = m_vertexDeclarationFlags & MODEL_TEXTURE2 ? sizeof(util::Vector<float, 2>) : 0;
      texStride[3] = m_vertexDeclarationFlags & MODEL_TEXTURE3 ? sizeof(util::Vector<float, 2>) : 0;
      GLuint normalStride     = m_vertexDeclarationFlags & MODEL_NORMAL        ? sizeof(util::Vector<float, 3>) : 0;
      GLuint binormalStride   = m_vertexDeclarationFlags & MODEL_BINORMAL      ? sizeof(util::Vector<float, 3>) : 0;
      GLuint boneWeightStride = m_vertexDeclarationFlags & MODEL_BONE_WEIGHTS  ? sizeof(util::Vector<float, 4>) : 0;

      GLuint lokalStride = posStride + texStride[0] + texStride[1] + texStride[2] + texStride[3] + normalStride + binormalStride + boneWeightStride;

      for(unsigned int i = 0; i < boneIndices.size(); i++)
		  {
        std::copy((GLubyte*)&boneIndices[i], (GLubyte*)&boneIndices[i] + sizeof(boneIndices[0]), &m_geometryData[0] + m_vertexStride * i);
      }
    }

    void Mesh::setVertexColors(std::vector<util::Vector<float, 4>> vertexColors)
    {
      assert(m_vertexDeclarationFlags & MODEL_COLOR);

      GLuint texStride[4];

      GLuint posStride        = m_vertexDeclarationFlags & MODEL_POSITION      ? sizeof(util::Vector<float, 3>) : 0;
      texStride[0] = m_vertexDeclarationFlags & MODEL_TEXTURE0 ? sizeof(util::Vector<float, 2>) : 0;
      texStride[1] = m_vertexDeclarationFlags & MODEL_TEXTURE1 ? sizeof(util::Vector<float, 2>) : 0;
      texStride[2] = m_vertexDeclarationFlags & MODEL_TEXTURE2 ? sizeof(util::Vector<float, 2>) : 0;
      texStride[3] = m_vertexDeclarationFlags & MODEL_TEXTURE3 ? sizeof(util::Vector<float, 2>) : 0;
      GLuint normalStride     = m_vertexDeclarationFlags & MODEL_NORMAL        ? sizeof(util::Vector<float, 3>) : 0;
      GLuint binormalStride   = m_vertexDeclarationFlags & MODEL_BINORMAL      ? sizeof(util::Vector<float, 3>) : 0;
      GLuint boneWeightStride = m_vertexDeclarationFlags & MODEL_BONE_WEIGHTS  ? sizeof(util::Vector<float, 4>) : 0;
      GLuint boneIndextStride = m_vertexDeclarationFlags & MODEL_BONE_INDICES  ? sizeof(util::Vector<float, 4>) : 0;

      GLuint lokalStride = posStride + texStride[0] + texStride[1] + texStride[2] + texStride[3] + normalStride + binormalStride + boneWeightStride + boneIndextStride;

      for(unsigned int i = 0; i < vertexColors.size(); i++)
		  {
        std::copy((GLubyte*)&vertexColors[i], (GLubyte*)&vertexColors[i] + sizeof(vertexColors[0]), &m_geometryData[0] + m_vertexStride * i);
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

    util::Vector<float, 3> Mesh::getBBMin() const
    {
      return m_boundingVolume.getBBMin();
    }

    util::Vector<float, 3> Mesh::getBBMax() const
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
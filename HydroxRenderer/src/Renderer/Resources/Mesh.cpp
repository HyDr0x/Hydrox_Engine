#include "Renderer/Resources/Mesh.h"

namespace he
{
	namespace renderer
	{
	

    Mesh::Mesh()
    {
      m_geometryData = 0;
      m_indexData = 0;

      m_primitiveCount = 0;
      m_vertexCount = 0;
      m_vertexStride = 0;
      m_vertexDeclarationFlags = 0;
      m_primitiveType = GL_TRIANGLES;
      m_verticesPerPrimitive = 3;
    }

    Mesh::Mesh(GLuint vertexDeclarationFlags, 
        std::vector<util::Vector<float, 3>> positions, 
        GLuint primitiveType,
        std::vector<indexType> indices,
        std::vector<util::Vector<float, 2>> textureCoords, 
        std::vector<util::Vector<float, 3>> normals, 
        std::vector<util::Vector<float, 3>> binormals, 
        std::vector<util::Vector<float, 4>> boneWeights,
        std::vector<util::Vector<float, 4>> boneIndices, 
        std::vector<util::Vector<float, 4>> vertexColors) : m_boundingVolume(positions)
    {
      std::vector<util::Vector<float, 3>> data(positions.size() * sizeof(util::Vector<float, 3>) + 
                                         textureCoords.size() * sizeof(util::Vector<float, 2>) + 
                                         normals.size() * sizeof(util::Vector<float, 3>) + 
                                         binormals.size() * sizeof(util::Vector<float, 3>) + 
                                         boneWeights.size() * sizeof(util::Vector<float, 4>) +
                                         boneIndices.size() * sizeof(util::Vector<float, 4>) + 
                                         vertexColors.size() * sizeof(util::Vector<float, 4>));
      unsigned int offset = 0;
      unsigned int length = positions.size() * sizeof(util::Vector<float, 3>);
      memcpy(&data[offset], &positions[0], length);
      offset += length;
      length = textureCoords.size() * sizeof(util::Vector<float, 2>);
      if(length) memcpy(&data[offset], &textureCoords[0], length);
      offset += length;
      length = normals.size() * sizeof(util::Vector<float, 3>);
      if(length) memcpy(&data[offset], &normals[0], length);
      offset += length;
      length = binormals.size() * sizeof(util::Vector<float, 3>);
      if(length) memcpy(&data[offset], &binormals[0], length);
      offset += length;
      length = boneWeights.size() * sizeof(util::Vector<float, 4>);
      if(length) memcpy(&data[offset], &boneWeights[0], length);
      offset += length;
      length = boneIndices.size() * sizeof(util::Vector<float, 4>);
      if(length) memcpy(&data[offset], &boneIndices[0], length);
      offset += length;
      length = vertexColors.size() * sizeof(util::Vector<float, 4>);
      if(length) memcpy(&data[offset], &vertexColors[0], length);

      offset += length;
      m_hash = MurmurHash64A(&data[0], offset, 0);
    
      m_primitiveType = primitiveType;

      switch(m_primitiveType)
      {
      case GL_POINTS:
        m_verticesPerPrimitive = 1;
        break;
      case GL_LINES:
        m_verticesPerPrimitive = 2;
        break;
      case GL_TRIANGLES:
      default:
        m_verticesPerPrimitive = 3;
        break;
      }

      m_primitiveCount = static_cast<unsigned int>(indices.size()) / m_verticesPerPrimitive;

      m_vertexCount = static_cast<unsigned int>(positions.size());
      m_vertexDeclarationFlags = vertexDeclarationFlags;

      GLuint posStride         = m_vertexDeclarationFlags & MODEL_POSITION      ? sizeof(util::Vector<float, 3>) : 0;
      GLuint texStride         = m_vertexDeclarationFlags & MODEL_TEXTURE       ? sizeof(util::Vector<float, 2>) : 0;
      GLuint normalStride      = m_vertexDeclarationFlags & MODEL_NORMAL        ? sizeof(util::Vector<float, 3>) : 0;
      GLuint binormalStride    = m_vertexDeclarationFlags & MODEL_BINORMAL      ? sizeof(util::Vector<float, 3>) : 0;
      GLuint boneWeightStride  = m_vertexDeclarationFlags & MODEL_BONE_WEIGHTS  ? sizeof(util::Vector<float, 4>) : 0;
      GLuint boneIndexStride   = m_vertexDeclarationFlags & MODEL_BONE_INDICES  ? sizeof(util::Vector<float, 4>) : 0;
      GLuint vertexColorStride = m_vertexDeclarationFlags & MODEL_COLOR         ? sizeof(util::Vector<float, 4>) : 0;

      m_vertexStride = posStride + texStride + normalStride + binormalStride + boneWeightStride + boneIndexStride + vertexColorStride;

      m_vertexStride += m_vertexStride % 4;

      GLint size = m_vertexCount * m_vertexStride;
      GLuint lokalStride = 0;

      glGenBuffers(1, &m_geometryData);

      glBindBuffer(GL_ARRAY_BUFFER, m_geometryData);
	    glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW);

      for(unsigned int i = 0; i < positions.size(); i++)
		  {
        glBufferSubData(GL_ARRAY_BUFFER, lokalStride + m_vertexStride * i, sizeof(util::Vector<float, 3>), &positions[i]);
      }
      lokalStride += posStride;

      for(unsigned int i = 0; i < textureCoords.size(); i++)
		  {
		    glBufferSubData(GL_ARRAY_BUFFER, lokalStride + m_vertexStride * i, sizeof(util::Vector<float, 2>), &textureCoords[i]);
      }
      lokalStride += texStride;

      for(unsigned int i = 0; i < normals.size(); i++)
		  {
		    glBufferSubData(GL_ARRAY_BUFFER, lokalStride + m_vertexStride * i, sizeof(util::Vector<float, 3>), &normals[i]);
      }
      lokalStride += normalStride;

      for(unsigned int i = 0; i < binormals.size(); i++)
		  {
        glBufferSubData(GL_ARRAY_BUFFER, lokalStride + m_vertexStride * i, sizeof(util::Vector<float, 3>), &binormals[i]);
      }
      lokalStride += binormalStride;

      for(unsigned int i = 0; i < boneWeights.size(); i++)
		  {
        glBufferSubData(GL_ARRAY_BUFFER, lokalStride + m_vertexStride * i, sizeof(util::Vector<float, 4>), &boneWeights[i]);
      }
      lokalStride += boneWeightStride;

      for(unsigned int i = 0; i < boneIndices.size(); i++)
		  {
        glBufferSubData(GL_ARRAY_BUFFER, lokalStride + m_vertexStride * i, sizeof(util::Vector<float, 4>), &boneIndices[i]);
      }
      lokalStride += boneIndexStride;

      for(unsigned int i = 0; i < vertexColors.size(); i++)
		  {
        glBufferSubData(GL_ARRAY_BUFFER, lokalStride + m_vertexStride * i, sizeof(util::Vector<float, 4>), &vertexColors[i]);
      }
      lokalStride += vertexColorStride;

	    glBindBuffer(GL_ARRAY_BUFFER, 0);

	    if(indices.size() > 0)
	    {
		    glGenBuffers(1, &m_indexData);
		    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexData);
			    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indexType), &indices[0], GL_STATIC_DRAW);
		    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	    }
	    else
      {
		    m_indexData = 0;
      }

	    //const GLenum ErrorValue = glGetError();
	    //int tmpE = 0;
	    //if(ErrorValue != GL_NO_ERROR)
	    //	tmpE++;
	    //tmpE = GL_INVALID_VALUE & GL_INVALID_VALUE;
    }

    Mesh::Mesh(const Mesh& o)
    {
      m_hash = o.m_hash;
      m_boundingVolume = o.m_boundingVolume;
      m_primitiveType = o.m_primitiveType;
      m_verticesPerPrimitive = o.m_verticesPerPrimitive;
	    m_primitiveCount = o.m_primitiveCount;
      m_vertexCount = o.m_vertexCount;
      m_vertexStride = o.m_vertexStride;
      m_geometryData = o.m_geometryData;
      m_indexData = o.m_indexData;
      m_vertexDeclarationFlags = o.m_vertexDeclarationFlags;
    }

    Mesh& Mesh::operator=(const Mesh& o)
    {
      m_hash = o.m_hash;
      m_boundingVolume = o.m_boundingVolume;
      m_primitiveType = o.m_primitiveType;
      m_verticesPerPrimitive = o.m_verticesPerPrimitive;
	    m_primitiveCount = o.m_primitiveCount;
      m_vertexCount = o.m_vertexCount;
      m_vertexStride = o.m_vertexStride;
      m_geometryData = o.m_geometryData;
      m_indexData = o.m_indexData;
      m_vertexDeclarationFlags = o.m_vertexDeclarationFlags;

      return *this;
    }

    Mesh::~Mesh()
    {
    }

    void Mesh::free()
    {
      glDeleteBuffers(1, &m_geometryData);
      glDeleteBuffers(1, &m_indexData);
    }

    void Mesh::render(GLuint bindingIndex) const
    {
      glBindVertexBuffer(bindingIndex, m_geometryData, 0, m_vertexStride);
	    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexData);
	    glDrawElements(m_primitiveType, m_primitiveCount * m_verticesPerPrimitive, GLINDEXTYPE, 0);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void Mesh::setPositions(std::vector<util::Vector<float, 3>> positions)
    {
      assert(m_vertexDeclarationFlags & MODEL_POSITION);

      glBindBuffer(GL_ARRAY_BUFFER, m_geometryData);

      for(unsigned int i = 0; i < positions.size(); i++)
		  {
        glBufferSubData(GL_ARRAY_BUFFER, m_vertexStride * i, sizeof(positions[0]), &positions[i]);
      }

      glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void Mesh::setTextureCoordinations(std::vector<util::Vector<float, 2>> textureCoords)
    {
      assert(m_vertexDeclarationFlags & MODEL_TEXTURE);

      GLuint posStride = m_vertexDeclarationFlags & MODEL_POSITION ? sizeof(util::Vector<float, 3>) : 0;

      GLuint lokalStride = posStride;

      glBindBuffer(GL_ARRAY_BUFFER, m_geometryData);

      for(unsigned int i = 0; i < textureCoords.size(); i++)
		  {
        glBufferSubData(GL_ARRAY_BUFFER, lokalStride + m_vertexStride * i, sizeof(textureCoords[0]), &textureCoords[i]);
      }

      glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void Mesh::setNormals(std::vector<util::Vector<float, 3>> normals)
    {
      assert(m_vertexDeclarationFlags & MODEL_NORMAL);

      GLuint posStride = m_vertexDeclarationFlags & MODEL_POSITION ? sizeof(util::Vector<float, 3>) : 0;
      GLuint texStride = m_vertexDeclarationFlags & MODEL_TEXTURE  ? sizeof(util::Vector<float, 2>) : 0;

      GLuint lokalStride = posStride + texStride;

      glBindBuffer(GL_ARRAY_BUFFER, m_geometryData);

      for(unsigned int i = 0; i < normals.size(); i++)
		  {
        glBufferSubData(GL_ARRAY_BUFFER, lokalStride +  m_vertexStride * i, sizeof(normals[0]), &normals[i]);
      }

      glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void Mesh::setBiNormals(std::vector<util::Vector<float, 3>> binormals)
    {
      assert(m_vertexDeclarationFlags & MODEL_BINORMAL);

      GLuint posStride     = m_vertexDeclarationFlags & MODEL_POSITION ? sizeof(util::Vector<float, 3>) : 0;
      GLuint texStride     = m_vertexDeclarationFlags & MODEL_TEXTURE  ? sizeof(util::Vector<float, 2>) : 0;
      GLuint normalStride  = m_vertexDeclarationFlags & MODEL_NORMAL   ? sizeof(util::Vector<float, 3>) : 0;

      GLuint lokalStride = posStride + texStride + normalStride;

      glBindBuffer(GL_ARRAY_BUFFER, m_geometryData);

      for(unsigned int i = 0; i < binormals.size(); i++)
		  {
        glBufferSubData(GL_ARRAY_BUFFER, lokalStride + m_vertexStride * i, sizeof(binormals[0]), &binormals[i]);
      }

      glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void Mesh::setBoneWeights(std::vector<util::Vector<float, 4>> boneWeights)
    {
      assert(m_vertexDeclarationFlags & MODEL_BONE_WEIGHTS);

      GLuint posStride      = m_vertexDeclarationFlags & MODEL_POSITION ? sizeof(util::Vector<float, 3>) : 0;
      GLuint texStride      = m_vertexDeclarationFlags & MODEL_TEXTURE  ? sizeof(util::Vector<float, 2>) : 0;
      GLuint normalStride   = m_vertexDeclarationFlags & MODEL_NORMAL   ? sizeof(util::Vector<float, 3>) : 0;
      GLuint binormalStride = m_vertexDeclarationFlags & MODEL_BINORMAL ? sizeof(util::Vector<float, 3>) : 0;

      GLuint lokalStride = posStride + texStride + normalStride + binormalStride;

      glBindBuffer(GL_ARRAY_BUFFER, m_geometryData);

      for(unsigned int i = 0; i < boneWeights.size(); i++)
		  {
        glBufferSubData(GL_ARRAY_BUFFER, lokalStride + m_vertexStride * i, sizeof(boneWeights[0]), &boneWeights[i]);
      }

      glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void Mesh::setBoneIndices(std::vector<util::Vector<float, 4>> boneIndices)
    {
      assert(m_vertexDeclarationFlags & MODEL_BONE_INDICES);

      GLuint posStride        = m_vertexDeclarationFlags & MODEL_POSITION      ? sizeof(util::Vector<float, 3>) : 0;
      GLuint texStride        = m_vertexDeclarationFlags & MODEL_TEXTURE       ? sizeof(util::Vector<float, 2>) : 0;
      GLuint normalStride     = m_vertexDeclarationFlags & MODEL_NORMAL        ? sizeof(util::Vector<float, 3>) : 0;
      GLuint binormalStride   = m_vertexDeclarationFlags & MODEL_BINORMAL      ? sizeof(util::Vector<float, 3>) : 0;
      GLuint boneWeightStride = m_vertexDeclarationFlags & MODEL_BONE_WEIGHTS  ? sizeof(util::Vector<float, 4>) : 0;

      GLuint lokalStride = posStride + texStride + normalStride + binormalStride + boneWeightStride;

      glBindBuffer(GL_ARRAY_BUFFER, m_geometryData);

      for(unsigned int i = 0; i < boneIndices.size(); i++)
		  {
        glBufferSubData(GL_ARRAY_BUFFER, lokalStride + m_vertexStride * i, sizeof(boneIndices[0]), &boneIndices[i]);
      }

      glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void Mesh::setVertexColors(std::vector<util::Vector<float, 4>> vertexColors)
    {
      assert(m_vertexDeclarationFlags & MODEL_COLOR);

      GLuint posStride        = m_vertexDeclarationFlags & MODEL_POSITION      ? sizeof(util::Vector<float, 3>) : 0;
      GLuint texStride        = m_vertexDeclarationFlags & MODEL_TEXTURE       ? sizeof(util::Vector<float, 2>) : 0;
      GLuint normalStride     = m_vertexDeclarationFlags & MODEL_NORMAL        ? sizeof(util::Vector<float, 3>) : 0;
      GLuint binormalStride   = m_vertexDeclarationFlags & MODEL_BINORMAL      ? sizeof(util::Vector<float, 3>) : 0;
      GLuint boneWeightStride = m_vertexDeclarationFlags & MODEL_BONE_WEIGHTS  ? sizeof(util::Vector<float, 4>) : 0;
      GLuint boneIndextStride = m_vertexDeclarationFlags & MODEL_BONE_INDICES  ? sizeof(util::Vector<float, 4>) : 0;

      GLuint lokalStride = posStride + texStride + normalStride + binormalStride + boneWeightStride + boneIndextStride;

      glBindBuffer(GL_ARRAY_BUFFER, m_geometryData);

      for(unsigned int i = 0; i < vertexColors.size(); i++)
		  {
        glBufferSubData(GL_ARRAY_BUFFER, lokalStride + m_vertexStride * i, sizeof(vertexColors[0]), &vertexColors[i]);
      }

      glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    GLuint Mesh::getVertexDeclarationFlags() const
    {
      return m_vertexDeclarationFlags;
    }

    GLuint Mesh::getPrimitiveType() const
    {
      return m_primitiveType;
    }

    util::Vector<float, 3> Mesh::getBBMin()
    {
      return m_boundingVolume.getBBMin();
    }

    util::Vector<float, 3> Mesh::getBBMax()
    {
      return m_boundingVolume.getBBMax();
    }

	}
}
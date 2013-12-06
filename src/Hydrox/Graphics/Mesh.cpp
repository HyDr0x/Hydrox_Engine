#include "Hydrox/Graphics/Mesh.h"

namespace he
{
  Mesh::Mesh()
  {
    m_geometryData = 0;
    m_indexData = 0;

    m_materialIndex = 0;
    m_primitiveCount = 0;
    m_vertexCount = 0;
    m_vertexStride = 0;
    m_vertexDeclarationFlags = 0;
    m_primitiveType = GL_TRIANGLES;
    m_verticesPerPrimitive = 3;
  }

  Mesh::Mesh(GLuint vertexDeclarationFlags, ResourceHandle materialIndex, 
              std::vector<Vector<float, 3>> positions, 
              std::vector<Vector<float, 2>> textureCoords, 
              std::vector<Vector<float, 3>> normals, 
              std::vector<Vector<float, 3>> binormals, 
              std::vector<Vector<float, 4>> boneIndices, 
              std::vector<Vector<float, 4>> boneWeights,
              std::vector<indexType> indices,
              GLuint primitiveType)
  {
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
    m_materialIndex = materialIndex;

    GLuint posStride        = m_vertexDeclarationFlags & MODEL_POSITION      ? sizeof(GLfloat) * 3 : 0;
    GLuint texStride        = m_vertexDeclarationFlags & MODEL_TEXTURE       ? sizeof(GLfloat) * 2 : 0;
    GLuint normalStride     = m_vertexDeclarationFlags & MODEL_NORMAL        ? sizeof(GLfloat) * 3 : 0;
    GLuint binormalStride   = m_vertexDeclarationFlags & MODEL_BINORMAL      ? sizeof(GLfloat) * 3 : 0;
    GLuint boneWeightStride = m_vertexDeclarationFlags & MODEL_BONE_WEIGHTS  ? sizeof(GLfloat) * 4 : 0;
    GLuint boneIndexStride  = m_vertexDeclarationFlags & MODEL_BONE_INDICES  ? sizeof(GLfloat) * 4 : 0;

    m_vertexStride = posStride + texStride + normalStride + binormalStride + boneWeightStride + boneIndexStride;

    m_vertexStride += m_vertexStride % 4;

    GLint size = m_vertexCount * m_vertexStride;
    GLuint lokalStride = 0;

    glGenBuffers(1, &m_geometryData);

    glBindBuffer(GL_ARRAY_BUFFER, m_geometryData);
	  glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW);

	  if(positions.size() > 0)
	  {
      for(unsigned int i = 0; i < m_vertexCount; i++)
		  {
        glBufferSubData(GL_ARRAY_BUFFER, lokalStride + m_vertexStride * i, sizeof(positions[0]), &positions[i]);
      }
      lokalStride += posStride;
	  }

	  if(textureCoords.size() > 0)
	  {
      for(unsigned int i = 0; i < m_vertexCount; i++)
		  {
		    glBufferSubData(GL_ARRAY_BUFFER, lokalStride + m_vertexStride * i, sizeof(textureCoords[0]), &textureCoords[i]);
      }
      lokalStride += texStride;
	  }

	  if(normals.size() > 0)
	  {
      for(unsigned int i = 0; i < m_vertexCount; i++)
		  {
		    glBufferSubData(GL_ARRAY_BUFFER, lokalStride + m_vertexStride * i, sizeof(normals[0]), &normals[i]);
      }
      lokalStride += normalStride;
	  }

	  if(binormals.size() > 0)
	  {
      for(unsigned int i = 0; i < m_vertexCount; i++)
		  {
        glBufferSubData(GL_ARRAY_BUFFER, lokalStride + m_vertexStride * i, sizeof(binormals[0]), &binormals[i]);
      }
      lokalStride += binormalStride;
	  }

	  if(boneIndices.size() > 0)
	  {
      for(int i = 0; i < m_vertexCount; i++)
		  {
        glBufferSubData(GL_ARRAY_BUFFER, lokalStride + m_vertexStride * i, sizeof(boneWeights[0]), &boneWeights[i]);
      }
      lokalStride += boneWeightStride;

      for(int i = 0; i < m_vertexCount; i++)
		  {
        glBufferSubData(GL_ARRAY_BUFFER, lokalStride + m_vertexStride * i, sizeof(boneIndices[0]), &boneIndices[i]);
      }
      lokalStride += boneIndexStride;
	  }

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
    m_materialIndex = o.m_materialIndex;
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
    m_materialIndex = o.m_materialIndex;
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

  void Mesh::setMaterial(ResourceHandle materialIndex)
  {
    m_materialIndex = materialIndex;
  }

  ResourceHandle Mesh::getMaterial() const
  {
    return m_materialIndex;
  }

  GLuint Mesh::getVertexDeclarationFlags() const
  {
    return m_vertexDeclarationFlags;
  }
}
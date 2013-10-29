#include "Hydrox/Graphics/Mesh.h"


Mesh::Mesh()
{
  m_geometryData = 0;
  m_indexData = 0;

  m_materialIndex = 0;
  m_triangleCount = 0;
  m_vertexCount = 0;
  m_vertexStride = 0;
  m_vertexDeclarationFlags = 0;
}

Mesh::Mesh(const Mesh& o)
{
  m_geometryData = o.m_geometryData;
  m_indexData = o.m_indexData;

  m_materialIndex = o.m_materialIndex;
  m_triangleCount = o.m_triangleCount;
  m_vertexCount = o.m_vertexCount;
  m_vertexStride = o.m_vertexStride;
  m_vertexDeclarationFlags = o.m_vertexDeclarationFlags;
}

Mesh::Mesh(GLuint vertexDeclarationFlags, ResourceHandle materialIndex, 
            std::vector<Vec<float, 3>> positions, 
            std::vector<Vec<float, 2>> textureCoords, 
            std::vector<Vec<float, 3>> normals, 
            std::vector<Vec<float, 3>> binormals, 
            std::vector<Vec<float, 4>> boneIndices, 
            std::vector<Vec<float, 4>> boneWeights,
            std::vector<indexType> indices)
{
  m_triangleCount = static_cast<unsigned int>(indices.size()) / 3;
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

	if(boneIndices.size() > 0)//TODO ANIMATION
	{
  //  for(int i = 0; i < m_vertexCount; i++)
		//{
  //    glBufferSubData(GL_ARRAY_BUFFER, lokalStride + m_vertexStride * i, boneWeightStride, &Mesh->mBones[i]->mWeights->mWeight);
  //  }
  //  lokalStride += boneWeightStride;

  //  for(int i = 0; i < m_vertexCount; i++)
		//{
  //    glBufferSubData(GL_ARRAY_BUFFER, lokalStride + m_vertexStride * i, boneIndexStride, &Mesh->mBones[i]->mWeights->mVertexId);
  //  }
  //  lokalStride += boneIndexStride;
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
		m_indexData = ~0;
  }

	//const GLenum ErrorValue = glGetError();
	//int tmpE = 0;
	//if(ErrorValue != GL_NO_ERROR)
	//	tmpE++;
	//tmpE = GL_INVALID_VALUE & GL_INVALID_VALUE;
}

Mesh& Mesh::operator=(const Mesh& o)
{
  m_materialIndex = o.m_materialIndex;

	m_triangleCount = o.m_triangleCount;
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
	glDrawElements(GL_TRIANGLES, m_triangleCount * 3, GLINDEXTYPE, 0);
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

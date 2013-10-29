#ifndef MESH_H_
#define MESH_H_

#include <vector>
#include <assert.h>

#include <GL/glew.h>

#include "Hydrox/DLLExport.h"

#include "Hydrox/Utility/Math/Math.hpp"

#include "Hydrox/Services/CachedResource.h"

#define GLINDEXTYPE GL_UNSIGNED_INT


class GRAPHICAPI Mesh : public CachedResource
{
public:

  enum VertexDeclarationFlags
  {
    MODEL_POSITION     = 1,
    MODEL_TEXTURE      = 2,
    MODEL_NORMAL       = 4,
    MODEL_BINORMAL     = 8,
    MODEL_BONE_WEIGHTS = 16,
    MODEL_BONE_INDICES = 32,
  };

  typedef GLuint indexType;

  Mesh();
	Mesh(const Mesh& o);

	Mesh(GLuint vertexDeclarationFlags, ResourceHandle materialIndex, 
    std::vector<Vec<float, 3>> positions, 
    std::vector<Vec<float, 2>> textureCoords, 
    std::vector<Vec<float, 3>> normals, 
    std::vector<Vec<float, 3>> binormals, 
    std::vector<Vec<float, 4>> boneIndices, 
    std::vector<Vec<float, 4>> boneWeights,
    std::vector<indexType> indices);

  Mesh& operator=(const Mesh& o);

	~Mesh();

  void free();

	void render(GLuint bindingIndex) const;

  void setMaterial(ResourceHandle materialIndex);
  ResourceHandle getMaterial() const;

  GLuint getVertexDeclarationFlags() const;

private:

  ResourceHandle m_materialIndex;

	////////////GEOMETRY////////////
	unsigned int m_triangleCount;
  unsigned int m_vertexCount;
  GLuint m_vertexStride;
  GLuint m_geometryData;
  GLuint m_indexData;
  GLuint m_vertexDeclarationFlags;
	////////////////////////////////
};

#endif
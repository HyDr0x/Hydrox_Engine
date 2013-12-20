#ifndef MESH_H_
#define MESH_H_

#include <vector>
#include <assert.h>

#include <GL/glew.h>

#include "Hydrox/DLLExport.h"

#include "Hydrox/Utility/Math/Math.hpp"

#include "Hydrox/Utility/Miscellaneous/CachedResource.h"
#include "Hydrox/Utility/Miscellaneous/ResourceHandle.h"

#define GLINDEXTYPE GL_UNSIGNED_INT

namespace he
{
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

	  Mesh(GLuint vertexDeclarationFlags, 
      std::vector<Vector<float, 3>> positions, 
      GLuint primitiveType = GL_TRIANGLES,
      std::vector<indexType> indices = std::vector<indexType>(),
      std::vector<Vector<float, 2>> textureCoords = std::vector<Vector<float, 2>>(), 
      std::vector<Vector<float, 3>> normals = std::vector<Vector<float, 3>>(), 
      std::vector<Vector<float, 3>> binormals = std::vector<Vector<float, 3>>(), 
      std::vector<Vector<float, 4>> boneIndices = std::vector<Vector<float, 4>>(), 
      std::vector<Vector<float, 4>> boneWeights = std::vector<Vector<float, 4>>()
      );
    
    Mesh& operator=(const Mesh& o);

	  ~Mesh();

    void free();

	  void render(GLuint bindingIndex) const;

    void setPositions(std::vector<Vector<float, 3>> positions);

    GLuint getVertexDeclarationFlags() const;
    GLuint getPrimitiveType() const;

  private:

    GLuint m_primitiveType;//which type of primitives? (for example GL_TRIANGLES, GL_LINES, GL_POINTS)
    GLuint m_verticesPerPrimitive;

	  ////////////GEOMETRY////////////
	  unsigned int m_primitiveCount;
    unsigned int m_vertexCount;
    GLuint m_vertexStride;
    GLuint m_geometryData;
    GLuint m_indexData;
    GLuint m_vertexDeclarationFlags;
	  ////////////////////////////////
  };
}

#endif
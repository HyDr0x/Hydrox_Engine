#ifndef MESH_H_
#define MESH_H_

#include <vector>
#include <assert.h>

#include <GL/glew.h>

#include "DataBase/DLLExport.h"

#include <Utilities/Math/Math.hpp>

#include "DataBase/ManagedResource.h"
#include <Utilities/Miscellaneous/ResourceHandle.h>

#include "DataBase/AABB.h"

#define GLINDEXTYPE GL_UNSIGNED_INT

namespace he
{
  namespace db
  {
    class GRAPHICAPI Mesh : public ManagedResource
    {
    public:

      enum VertexDeclarationFlags
      {
        MODEL_POSITION     = 1,
        MODEL_TEXTURE0     = 2,
        MODEL_TEXTURE1     = 4,
        MODEL_TEXTURE2     = 8,
        MODEL_TEXTURE3     = 16,
        MODEL_NORMAL       = 32,
        MODEL_BINORMAL     = 64,
        MODEL_BONE_WEIGHTS = 128,
        MODEL_BONE_INDICES = 256,
        MODEL_COLOR        = 512,
      };

      typedef GLuint indexType;

      Mesh();
      Mesh(GLenum primitiveType,
           const std::vector<util::vec3f>& positions, 
           const std::vector<indexType>& indices = std::vector<indexType>(),
           const std::vector<std::vector<util::vec2f>>& textureCoords = std::vector<std::vector<util::vec2f>>(4), 
           const std::vector<util::vec3f>& normals = std::vector<util::vec3f>(), 
           const std::vector<util::vec3f>& binormals = std::vector<util::vec3f>(), 
           const std::vector<util::vec4f>& boneWeights = std::vector<util::vec4f>(),
           const std::vector<util::vec4f>& boneIndices = std::vector<util::vec4f>(),
           const std::vector<util::vec4f>& vertexColors = std::vector<util::vec4f>()
           );

      Mesh(AABB boundingVolume,
           GLenum primitiveType,
           unsigned int primitiveCount,
           unsigned int vertexCount,
           GLuint vertexStride,
           GLuint vertexDeclarationFlags,
           const std::vector<GLubyte>& vboBuffer, 
           const std::vector<indexType>& indices = std::vector<indexType>()
           );

      Mesh(const Mesh& other);

      ~Mesh();

      Mesh& operator=(const Mesh& other);

      void free();

      void setPositions(std::vector<util::vec3f> positions);
      void setTextureCoordinations(std::vector<std::vector<util::vec2f>> textureCoords);
      void setNormals(std::vector<util::vec3f> normals);
      void setBiNormals(std::vector<util::vec3f> binormals);
      void setBoneWeights(std::vector<util::vec4f> boneWeights);
      void setBoneIndices(std::vector<util::vec4f> boneIndices);
      void setVertexColors(std::vector<util::vec4f> vertexColors);

      GLuint getVertexDeclarationFlags() const;
      GLuint getPrimitiveType() const;

      util::vec3f getBBMin() const;
      util::vec3f getBBMax() const;

      const std::vector<GLubyte>& getVBOBuffer() const;
      GLuint getVertexStride() const;
      GLuint getVertexCount() const;
      GLuint getVBOSize() const;

      const std::vector<indexType>& getIndexBuffer() const;
      GLuint getIndexCount() const;
      GLuint getPrimitiveCount() const;

    private:

      AABB m_boundingVolume;

      GLuint m_primitiveType;//which type of primitives? (for example GL_TRIANGLES, GL_LINES, GL_POINTS)

      ////////////GEOMETRY////////////
      unsigned int m_primitiveCount;
      unsigned int m_vertexCount;
      GLuint m_vertexStride;
      std::vector<GLubyte> m_geometryData;
      std::vector<indexType> m_indexData;
      GLuint m_vertexDeclarationFlags;
      ////////////////////////////////
    };
  }
}

#endif

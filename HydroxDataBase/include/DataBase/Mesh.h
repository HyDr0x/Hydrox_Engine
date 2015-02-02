#ifndef MESH_H_
#define MESH_H_

#include <vector>
#include <cassert>

#include <GL/glew.h>

#include <Utilities/Math/Math.hpp>
#include <Utilities/Miscellaneous/Flags.hpp>
#include <Utilities/Miscellaneous/CacheGenerator.h>
#include <Utilities/Miscellaneous/ResourceHandle.h>

#include "DataBase/DLLExport.h"
#include "DataBase/AABB.h"
#include "DataBase/ManagedResource.h"

#define GLINDEXTYPE GL_UNSIGNED_INT
#define GLCACHEINDEXTYPE GL_FLOAT

namespace he
{
  namespace db
  {
    class GRAPHICAPI Mesh : public ManagedResource
    {
    public:

      enum VertexDeclaration
      {
        MODEL_POSITION,
        MODEL_TEXTURE0,
        MODEL_TEXTURE1,
        MODEL_TEXTURE2,
        MODEL_TEXTURE3,
        MODEL_NORMAL,
        MODEL_BINORMAL,
        MODEL_BONE_WEIGHTS,
        MODEL_BONE_INDICES,
        MODEL_COLOR,
        MODEL_CACHEINDICES0,
        MODEL_CACHEINDICES1,
        SPECIAL0,
        SPECIAL1,
        SPECIAL2,
        VERTEXDECLARATIONFLAGNUMBER,
      };

      typedef GLuint indexType;
      
      Mesh();
      Mesh(GLenum primitiveType,
           const std::vector<util::vec3f>& positions, 
           const std::vector<util::Cache>& caches = std::vector<util::Cache>(),
           const std::vector<util::vec2ui>& triangleCacheIndices = std::vector<util::vec2ui>(),
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
           const std::vector<util::Cache>& caches,
           const std::vector<util::vec2ui>& triangleCacheIndices,
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

      util::Flags<VertexDeclaration> getVertexDeclarationFlags() const;
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
      GLuint getCacheCount() const;

      const std::vector<util::Cache>& getCaches() const;
      const std::vector<util::vec2ui>& getTriangleCacheIndices() const;

      static unsigned int vertexDeclarationSize(unsigned int index);

    private:

      void generateNormals(std::vector<util::vec3f>& outNormals, const std::vector<util::vec3f>& positions, const std::vector<indexType>& indices);
      //invalid indices are marked as 0
      void generateCacheIndizes(const std::vector<util::vec3f>& positions, const std::vector<util::vec3f>& normals, std::vector<util::cacheIndexType>& cacheIndizes0, std::vector<util::cacheIndexType>& cacheIndizes1);

      static const unsigned int VERTEXDECLARATIONSIZE[VERTEXDECLARATIONFLAGNUMBER];

      AABB m_boundingVolume;

      GLuint m_primitiveType;//which type of primitives? (for example GL_TRIANGLES, GL_LINES, GL_POINTS)

      ////////////GEOMETRY////////////
      unsigned int m_primitiveCount;
      unsigned int m_vertexCount;
      GLuint m_vertexStride;
      std::vector<GLubyte> m_geometryData;
      std::vector<util::Cache> m_cacheData;
      std::vector<util::vec2ui> m_triangleCacheIndices;
      std::vector<indexType> m_indexData;
      util::Flags<VertexDeclaration> m_vertexDeclaration;
      ////////////////////////////////
    };

    typedef db::Mesh::VertexDeclaration VertexDeclarationFlags;
  }
}

#endif

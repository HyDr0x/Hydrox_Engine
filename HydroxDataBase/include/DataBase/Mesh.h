#ifndef MESH_H_
#define MESH_H_

#include <vector>
#include <cassert>

#include <GL/glew.h>

#include <Utilities/Math/Math.hpp>
#include <Utilities/Math/Triangle.h>
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

      enum VertexDeclarationElements
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
      Mesh(GLenum primitiveType, unsigned int vertexCount, std::vector<VertexDeclarationElements> flags, std::vector<indexType> indices = std::vector<indexType>());

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

      Mesh& operator=(Mesh other); 

      void free();

      void generateBoundingVolume();
      void generateNormals();
      void generateCaches(float errorRate, float maxDistance, float maxAngle);

      void getDataFromGeometryBuffer(unsigned int vertexDeclaration, unsigned int offset, unsigned int numberOfElements, GLubyte *data) const;
      void copyDataIntoGeometryBuffer(unsigned int vertexDeclaration, unsigned int offset, unsigned int numberOfElements, const GLubyte *data);

      util::Flags<VertexDeclarationElements> getVertexDeclarationFlags() const;
      GLuint getPrimitiveType() const;

      const AABB& getBoundingVolume() const;

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
      void setCaches(const std::vector<util::vec3f>& points, const std::vector<util::vec3f>& pointNormals, const std::vector<util::Triangle>& pointTriangles);
      void addCache(const util::vec3f& point, const util::vec3f& pointNormal, const util::Triangle& triangle);

      const std::vector<util::vec2ui>& getTriangleCacheIndices() const;

      static unsigned int vertexDeclarationSize(unsigned int index);

    protected:

      virtual void updateHash();

    private:

      static const unsigned int VERTEXDECLARATIONSIZE[VERTEXDECLARATIONFLAGNUMBER];

      static const unsigned int CACHEINDEXSIZE = 8;

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
      util::Flags<VertexDeclarationElements> m_vertexDeclaration;
      ////////////////////////////////
    };
  }

  typedef db::Mesh::VertexDeclarationElements VertexElements;
  typedef util::Flags<VertexElements> VertexElementFlags;
}

#endif

#ifndef MESH_H_
#define MESH_H_

#include <vector>
#include <assert.h>

#include <GL/glew.h>

#include "Renderer/DLLExport.h"

#include <Utilities/Math/Math.hpp>

#include "Renderer/Resources/ManagedResource.h"
#include <Utilities/Miscellaneous/ResourceHandle.h>

#include "Renderer/Resources/AABB.h"

#define GLINDEXTYPE GL_UNSIGNED_INT

namespace he
{
  namespace renderer
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
           const std::vector<util::Vector<float, 3>>& positions, 
           const std::vector<indexType>& indices = std::vector<indexType>(),
           const std::vector<std::vector<util::Vector<float, 2>>>& textureCoords = std::vector<std::vector<util::Vector<float, 2>>>(4), 
           const std::vector<util::Vector<float, 3>>& normals = std::vector<util::Vector<float, 3>>(), 
           const std::vector<util::Vector<float, 3>>& binormals = std::vector<util::Vector<float, 3>>(), 
           const std::vector<util::Vector<float, 4>>& boneWeights = std::vector<util::Vector<float, 4>>(),
           const std::vector<util::Vector<float, 4>>& boneIndices = std::vector<util::Vector<float, 4>>(),
           const std::vector<util::Vector<float, 4>>& vertexColors = std::vector<util::Vector<float, 4>>()
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

      void setPositions(std::vector<util::Vector<float, 3>> positions);
      void setTextureCoordinations(std::vector<std::vector<util::Vector<float, 2>>> textureCoords);
      void setNormals(std::vector<util::Vector<float, 3>> normals);
      void setBiNormals(std::vector<util::Vector<float, 3>> binormals);
      void setBoneWeights(std::vector<util::Vector<float, 4>> boneWeights);
      void setBoneIndices(std::vector<util::Vector<float, 4>> boneIndices);
      void setVertexColors(std::vector<util::Vector<float, 4>> vertexColors);

      GLuint getVertexDeclarationFlags() const;
      GLuint getPrimitiveType() const;

      util::Vector<float, 3> getBBMin() const;
      util::Vector<float, 3> getBBMax() const;

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

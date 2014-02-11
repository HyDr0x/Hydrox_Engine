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
        MODEL_TEXTURE      = 2,
        MODEL_NORMAL       = 4,
        MODEL_BINORMAL     = 8,
        MODEL_BONE_WEIGHTS = 16,
        MODEL_BONE_INDICES = 32,
        MODEL_COLOR        = 64,
      };

      typedef GLuint indexType;

      Mesh();
	    Mesh(const Mesh& o);

	    Mesh(GLuint vertexDeclarationFlags, 
        std::vector<util::Vector<float, 3>> positions, 
        GLuint primitiveType = GL_TRIANGLES,
        std::vector<indexType> indices = std::vector<indexType>(),
        std::vector<util::Vector<float, 2>> textureCoords = std::vector<util::Vector<float, 2>>(), 
        std::vector<util::Vector<float, 3>> normals = std::vector<util::Vector<float, 3>>(), 
        std::vector<util::Vector<float, 3>> binormals = std::vector<util::Vector<float, 3>>(), 
        std::vector<util::Vector<float, 4>> boneWeights = std::vector<util::Vector<float, 4>>(),
        std::vector<util::Vector<float, 4>> boneIndices = std::vector<util::Vector<float, 4>>(),
        std::vector<util::Vector<float, 4>> vertexColors = std::vector<util::Vector<float, 4>>()
        );
    
      Mesh& operator=(const Mesh& o);

	    ~Mesh();

      void free();

	    void render(GLuint bindingIndex) const;

      void setPositions(std::vector<util::Vector<float, 3>> positions);
      void setTextureCoordinations(std::vector<util::Vector<float, 2>> textureCoords);
      void setNormals(std::vector<util::Vector<float, 3>> normals);
      void setBiNormals(std::vector<util::Vector<float, 3>> binormals);
      void setBoneWeights(std::vector<util::Vector<float, 4>> boneWeights);
      void setBoneIndices(std::vector<util::Vector<float, 4>> boneIndices);
      void setVertexColors(std::vector<util::Vector<float, 4>> vertexColors);

      GLuint getVertexDeclarationFlags() const;
      GLuint getPrimitiveType() const;

      util::Vector<float, 3> getBBMin();
      util::Vector<float, 3> getBBMax();

    private:

      AABB m_boundingVolume;

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
}

#endif

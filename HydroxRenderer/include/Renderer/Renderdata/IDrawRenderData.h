#ifndef IDRAWRENDERDATA_H_
#define IDRAWRENDERDATA_H_

#include <Utilities/Miscellaneous/ResourceHandle.h>
#include <Utilities/Miscellaneous/SingletonManager.hpp>

#include <XBar/IGeometryContainer.h>

#include <DataBase/ResourceManager.hpp>
#include <DataBase/Mesh.h>

#include "Renderer/Buffer/GPUImmutableBuffer.h"
#include "Renderer/Pipeline/RenderOptions.h"
#include "Renderer/Renderdata/IInstanceRenderData.h"

namespace he
{
  namespace renderer
  {
    class IDrawRenderData
    {
    public:

      IDrawRenderData(GLenum primitiveType, GLuint vertexStride, VertexElementFlags meshVertexDeclaration, util::SingletonManager *singletonManager) :
        m_primitiveType(primitiveType),
        m_vertexStride(vertexStride),
        m_meshVertexDeclaration(meshVertexDeclaration),
        m_instanceNumber(0),
        m_updateMeshData(false)
      {
        m_modelManager = singletonManager->getService<db::ModelManager>();
        m_options = singletonManager->getService<RenderOptions>();
      }

      virtual ~IDrawRenderData() = 0 {}

      virtual bool canBeInserted(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer) = 0;

      virtual void insertGeometry(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer) = 0;
      virtual void removeGeometry(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer) = 0;

      virtual void bindFrustumCullingBuffer() const = 0;
      virtual void unbindFrustumCullingBuffer() const = 0;

      virtual void bindOccluderBuffer() const = 0;
      virtual void unbindOccluderBuffer() const = 0;

      virtual void bindDrawBuffer() const = 0;
      virtual void unbindDrawBuffer() const = 0;

      virtual void draw() const = 0;

      virtual void updateBuffer(IInstanceRenderData& renderData) = 0;

      virtual unsigned int getPerInstanceOccluderNumber() const = 0;

      virtual unsigned int getPerInstanceVertexNumber() const = 0;

      virtual unsigned int getPerInstanceTriangleNumber() const = 0;

      virtual VertexElementFlags getMeshVertexDeclaration() const = 0;

      GLenum getPrimitiveType() const
      {
        return m_primitiveType;
      }

    protected:

      util::SharedPointer<db::ModelManager> m_modelManager;
      util::SharedPointer<RenderOptions> m_options;

      //per mesh buffer
      GPUImmutableBuffer m_meshVertexBuffer;
      GPUImmutableBuffer m_bboxesBuffer;
      GPUImmutableBuffer m_commandBuffer;
      GPUImmutableBuffer m_meshInstanceBufferIndex;
      GPUImmutableBuffer m_triangleIndexOffsetBuffer;//has the offsets into the triangleBuffer for the specific mesh

      GPUImmutableBuffer m_occluderInstanceOffsetBuffer;//the per instance offset for the global occluder index
      GPUImmutableBuffer m_occluderOffsetBuffer;//has the offsets into the occluderBuffer for the specific mesh
      GPUImmutableBuffer m_occluderBuffer;//has all occluders for all the meshes sorted to their triangles
      GPUImmutableBuffer m_occluderIndicesPerTriangleBuffer;//has the indices into the cacheBuffer for the triangle

      GLenum m_primitiveType;

      GLuint m_vertexStride;

      VertexElementFlags m_meshVertexDeclaration;

      unsigned int m_occluderNumber;
      unsigned int m_perInstanceOccluderNumber;

      unsigned int m_perInstanceVertexNumber;
      unsigned int m_perInstanceTriangleNumber;

      unsigned int m_instanceNumber;

      bool m_updateMeshData;

      std::map<util::ResourceHandle, uint64_t, util::ResourceHandle::Less> m_meshHashes;//original meshHandle with the actual hash value and data and the old hash value with the data uploaded to the gpu
    };
  }
}

#endif

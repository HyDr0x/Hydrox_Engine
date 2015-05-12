#ifndef DRAWELEMENTSDECORATOR_H_
#define DRAWELEMENTSDECORATOR_H_

#include "Renderer/TreeNodes/RenderNodeDecorator/ARenderNodeDecorator.h"
#include "Renderer/Buffer/GPUImmutableBuffer.h"
#include "Renderer/Pipeline/RenderOptions.h"

namespace he
{
  namespace db
  {
    class Mesh;
  }

  namespace renderer
  {
    struct DrawElementsIndirectCommand
    {
      GLuint count;//number of indices
      GLuint instanceCount;//the number of instances
      GLuint firstIndex;//offset in the index array
      GLuint baseVertex;//offset which adds to every index value
      GLuint baseInstance;//base instance, getting added to all vertex attribute divisors, not to gl_InstanceID!
    };

    struct ElementGeometry
    {
      unsigned int bufferIndex;
      unsigned int vertexOffset;
      unsigned int indexOffset;
      unsigned int instanceNumber;
    };

    class DrawElementsDecorator : public ARenderNodeDecorator
    {
    public:

      DrawElementsDecorator(IRenderGroup *renderNode, GLenum indexType, GLenum primitiveType, GLuint vertexStride, util::SingletonManager *singletonManager);
      virtual ~DrawElementsDecorator();

      virtual bool insertGeometry(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer);
      virtual bool removeGeometry(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer);

      virtual void frustumCulling() const;

      virtual void rasterizeShadowGeometry() const;

      virtual void rasterizeReflectiveShadowGeometry() const;

      virtual void rasterizeIndexGeometry() const;

      virtual void rasterizeIndirectLightingGeometry() const;

      virtual void rasterizeGeometry() const;

      virtual void updateBuffer();

      virtual unsigned int getCacheNumber() const;

    protected:

      void updatePerMeshBuffer();
      void updatePerInstanceBuffer();

      util::SharedPointer<db::ModelManager> m_modelManager;
      util::SharedPointer<RenderOptions> m_options;

      //per mesh buffer
      GPUImmutableBuffer m_meshVertexBuffer;
      GPUImmutableBuffer m_meshIndexBuffer;
      GPUImmutableBuffer m_bboxesBuffer;
      GPUImmutableBuffer m_commandBuffer;
      GPUImmutableBuffer m_meshInstanceBufferIndex;
      GPUImmutableBuffer m_cacheInstanceOffsetBuffer;//the per instance offset for the global cache index
      GPUImmutableBuffer m_triangleIndexOffsetBuffer;//has the offsets into the triangleBuffer for the specific mesh
      GPUImmutableBuffer m_cacheOffsetBuffer;//has the offsets into the cacheBuffer for the specific mesh
      GPUImmutableBuffer m_triangleBuffer;//has the indices into the cacheBuffer for the triangle
      GPUImmutableBuffer m_cacheBuffer;//has all caches for all the meshes sorted to their triangles

      GLenum m_primitiveType;

      GLuint m_vertexStride;

      GLenum m_indexType;

      bool m_updateMeshData;

      unsigned int m_cacheNumber;
      unsigned int m_perInstanceCacheNumber;

      std::map<util::ResourceHandle, ElementGeometry, util::ResourceHandle::Less> m_meshes;
      std::map<util::ResourceHandle, uint64_t, util::ResourceHandle::Less> m_meshHashes;//original meshHandle with the actual hash value and data and the old hash value with the data uploaded to the gpu
    };
  }
}

#endif
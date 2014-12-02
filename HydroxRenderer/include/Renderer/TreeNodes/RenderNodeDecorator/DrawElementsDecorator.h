#ifndef DRAWELEMENTSDECORATOR_H_
#define DRAWELEMENTSDECORATOR_H_

#include "Renderer/TreeNodes/RenderNodeDecorator/ARenderNodeDecorator.h"

#include "Renderer/Buffer/GPUImmutableBuffer.h"

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

      virtual bool insertGeometry(const xBar::IGeometryContainer& geometryContainer);
      virtual bool removeGeometry(const xBar::IGeometryContainer& geometryContainer);

      virtual void frustumCulling() const;

      virtual void rasterizeShadowGeometry() const;

      virtual void rasterizeIndexGeometry() const;

      virtual void rasterizeGeometry() const;

      virtual void updateBuffer();

      virtual unsigned int getCacheNumber() const;

    protected:

      void updatePerMeshBuffer();
      void updatePerInstanceBuffer();

      db::ModelManager *m_modelManager;

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

      bool m_meshNumberChanged;

      unsigned int m_vboSize;
      unsigned int m_iboSize;
      unsigned int m_triangleNumber;
      unsigned int m_cacheNumber;
      unsigned int m_perInstanceCacheNumber;

      std::map<util::ResourceHandle, ElementGeometry, Less> m_meshes;
    };
  }
}

#endif
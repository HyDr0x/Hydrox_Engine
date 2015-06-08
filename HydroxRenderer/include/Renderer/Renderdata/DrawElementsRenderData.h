#ifndef DRAWELEMENTSRENDERDATA_H_
#define DRAWELEMENTSRENDERDATA_H_

#include "Renderer/Renderdata/IDrawRenderData.h"

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

    class DrawElementsRenderData : public IDrawRenderData
    {
    public:

      DrawElementsRenderData(GLenum indexType, GLenum primitiveType, GLuint vertexStride, VertexElementFlags meshVertexDeclaration, util::SingletonManager *singletonManager);
      virtual ~DrawElementsRenderData();

      virtual bool canBeInserted(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer) override;

      virtual void insertGeometry(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer) override;
      virtual void removeGeometry(util::SharedPointer<const xBar::IGeometryContainer> geometryContainer) override;

      virtual void bindFrustumCullingBuffer() const override;
      virtual void unbindFrustumCullingBuffer() const override;

      virtual void bindCacheBuffer() const override;
      virtual void unbindCacheBuffer() const override;

      virtual void bindCacheInstanceOffsetBuffer() const override;
      virtual void unbindCacheInstanceOffsetBuffer() const override;

      virtual void bindDrawBuffer() const override;
      virtual void unbindDrawBuffer() const override;

      virtual void draw() const override;

      virtual void updateBuffer(IInstanceRenderData& renderData) override;

      virtual unsigned int getCacheNumber() const override;

      virtual VertexElementFlags getMeshVertexDeclaration() const override;

    protected:

      void updatePerMeshBuffer();
      void updatePerInstanceBuffer(IInstanceRenderData& renderData);

      //per mesh buffer
      GPUImmutableBuffer m_meshIndexBuffer;

      GLenum m_indexType;

      std::map<util::ResourceHandle, ElementGeometry, util::ResourceHandle::Less> m_meshes;
    };
  }
}

#endif
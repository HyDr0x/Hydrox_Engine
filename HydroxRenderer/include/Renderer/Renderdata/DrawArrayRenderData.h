#ifndef DRAWARRAYRENDERDATA_H_
#define DRAWARRAYRENDERDATA_H_

#include "Renderer/Renderdata/IDrawRenderData.h"

namespace he
{
  namespace db
  {
    class Mesh;
  }

  namespace renderer
  {
    struct DrawArraysIndirectCommand
    {
      GLuint count;//number of indices
      GLuint instanceCount;//the number of instances
      GLuint baseVertex;//offset which adds to every index value
      GLuint baseInstance;//base instance, getting added to all vertex attribute divisors, not to gl_InstanceID!
      GLuint padding;//padding, so array and elements command buffer can have the same size in the frustum culling shader
    };

    struct ArrayGeometry
    {
      unsigned int bufferIndex;
      unsigned int vertexOffset;
      unsigned int instanceNumber;
    };

    class DrawArrayRenderData : public IDrawRenderData
    {
    public:

      DrawArrayRenderData(GLenum primitiveType, GLuint vertexStride, VertexElementFlags meshVertexDeclaration, util::SingletonManager *singletonManager);
      virtual ~DrawArrayRenderData();

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

      std::map<util::ResourceHandle, ArrayGeometry, util::ResourceHandle::Less> m_meshes;
    };
  }
}

#endif
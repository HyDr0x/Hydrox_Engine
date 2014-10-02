#ifndef DRAWARRAYDECORATOR_H_
#define DRAWARRAYDECORATOR_H_

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

    class DrawArrayDecorator : public ARenderNodeDecorator
    {
    public:

      DrawArrayDecorator(IRenderNode *renderNode, GLenum primitiveType, GLuint vertexStride, util::SingletonManager *singletonManager);
      virtual ~DrawArrayDecorator();

      virtual bool insertGeometry(const xBar::IGeometryContainer& geometryContainer);
      virtual bool removeGeometry(const xBar::IGeometryContainer& geometryContainer);

      virtual void frustumCulling() const;

      virtual void rasterizeGeometry() const;

      virtual void updateBuffer();

    protected:

      void updatePerMeshBuffer();
      void updatePerInstanceBuffer();

      db::ModelManager *m_modelManager;

      //per mesh buffer
      GPUImmutableBuffer m_meshVertexBuffer;
      GPUImmutableBuffer m_bboxesBuffer;
      GPUImmutableBuffer m_commandBuffer;
      GPUImmutableBuffer m_meshInstanceBufferIndex;

      GLenum m_primitiveType;

      GLuint m_vertexStride;

      bool m_meshNumberChanged;

      unsigned int m_vboSize;

      std::map<util::ResourceHandle, ArrayGeometry, Less> m_meshes;
    };
  }
}

#endif
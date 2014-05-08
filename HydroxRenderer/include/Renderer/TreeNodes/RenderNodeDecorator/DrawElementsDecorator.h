#ifndef DRAWELEMENTSDECORATOR_H_
#define DRAWELEMENTSDECORATOR_H_

#include "Renderer/TreeNodes/RenderNodeDecorator/ARenderNodeDecorator.h"

#include "Renderer/Buffer/GPUImmutableBuffer.h"

namespace he
{
	namespace renderer
	{
    class Mesh;

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

      DrawElementsDecorator(IRenderNode *renderNode, GLenum indexType, GLenum primitiveType, GLuint vertexStride, util::SingletonManager *singletonManager);
      ~DrawElementsDecorator();

      virtual bool insertGeometry(xBar::SkinnedGeometryContainer& geometryContainer);

      virtual bool insertGeometry(xBar::StaticGeometryContainer& geometryContainer);
      virtual bool removeGeometry(xBar::StaticGeometryContainer& geometryContainer);

      virtual void frustumCulling();

      virtual void rasterizeGeometry();

      virtual void updateBuffer();

    protected:

      void createBuffer();
      void fillBuffer();
      void updateCommandBuffer();

      ModelManager *m_modelManager;

      //per mesh buffer
      GPUImmutableBuffer m_meshVertexBuffer;
      GPUImmutableBuffer m_meshIndexBuffer;
      GPUImmutableBuffer m_bboxesBuffer;
      GPUImmutableBuffer m_commandBuffer;
      GPUImmutableBuffer m_meshInstanceBufferIndex;

      GLenum m_primitiveType;

      GLuint m_vertexStride;

      GLenum m_indexType;

      bool m_meshNumberChanged;

      unsigned int m_vboSize;
      unsigned int m_iboSize;

      std::map<util::ResourceHandle, ElementGeometry, Less> m_meshes;
    };
  }
}

#endif
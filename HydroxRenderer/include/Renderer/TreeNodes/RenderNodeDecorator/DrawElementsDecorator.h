#ifndef DRAWELEMENTSDECORATOR_H_
#define DRAWELEMENTSDECORATOR_H_

#include "Renderer/TreeNodes/RenderNodeDecorator/ARenderNodeDecorator.h"

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

    class DrawElementsDecorator : public ARenderNodeDecorator
    {
    public:

      DrawElementsDecorator(IRenderNode *renderNode, bool instanced, GLenum indexType, GLenum primitiveType, GLuint vertexStride, util::SingletonManager *singletonManager);
      ~DrawElementsDecorator();

      virtual bool insertGeometry(xBar::StaticGeometryContainer& geometryContainer);
      virtual bool removeGeometry(xBar::StaticGeometryContainer& geometryContainer);

      virtual bool isInstanced();

      virtual void frustumCulling();

      virtual void rasterizeGeometry();

      virtual void updateBuffer();

    protected:

      void resizeBuffer();

      void createBuffer();
      void fillCaches();
      void fillBuffer();
      void updateCommandBuffer();

      ModelManager *m_modelManager;

      std::vector<DrawElementsIndirectCommand> m_commandCache;
      std::vector<util::Vector<float, 4>> m_boundingBoxCache;
      std::vector<GLuint> m_meshInstanceIndexCache;

      //per mesh buffer
      GPUBuffer m_commandBuffer;
      GPUBuffer m_meshVertexBuffer;
      GPUBuffer m_meshIndexBuffer;
      GPUBuffer m_bboxesBuffer;
      GPUBuffer m_meshInstanceIndexBuffer;

      GLenum m_primitiveType;

      bool m_instanced;

      unsigned int m_meshNumber;
      GLuint m_vertexStride;

      GLenum m_indexType;

      unsigned int m_vboSize;
      unsigned int m_iboSize;

      unsigned int m_indexCount;
      unsigned int m_indexOffset;

      unsigned int m_vertexOffset;
      unsigned int m_vertexCount;

      std::map<util::ResourceHandle, unsigned int, Less> m_instanceNumberPerMesh;

      ////////////////////////////////////////////////////////////////
    };
  }
}

#endif
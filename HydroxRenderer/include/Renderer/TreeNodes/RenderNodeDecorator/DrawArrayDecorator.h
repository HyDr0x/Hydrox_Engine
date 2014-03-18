#ifndef DRAWARRAYDECORATOR_H_
#define DRAWARRAYDECORATOR_H_

#include "Renderer/TreeNodes/RenderNodeDecorator/ARenderNodeDecorator.h"

namespace he
{
	namespace renderer
	{
    class Mesh;

    struct DrawArraysIndirectCommand
    {
      GLuint count;//number of indices
      GLuint instanceCount;//the number of instances
      GLuint baseVertex;//offset which adds to every index value
      GLuint baseInstance;//base instance, getting added to all vertex attribute divisors, not to gl_InstanceID!
    };

    class DrawArrayDecorator : public ARenderNodeDecorator
    {
    public:

      DrawArrayDecorator(IRenderNode *renderNode, bool instanced, GLenum primitiveType, GLuint vertexStride, util::SingletonManager *singletonManager);
      ~DrawArrayDecorator();

      virtual bool insertGeometry(xBar::StaticGeometryContainer& geometryContainer);
      virtual bool removeGeometry(xBar::StaticGeometryContainer& geometryContainer);

      virtual void rasterizeGeometry();

      virtual void updateBuffer();

    protected:

      void resizeBuffer();

      void createBuffer();
      void fillCaches();
      void fillBuffer();
      void updateCommandBuffer();

      ModelManager *m_modelManager;

      /////////////////////////DRAWING-INDEXED-NONINDEXED///////////////////////////

      std::vector<DrawArraysIndirectCommand> m_commandCache;
      std::vector<util::Vector<float, 4>> m_boundingBoxCache;

      //per mesh buffer
      GPUBuffer m_commandBuffer;
      GPUBuffer m_meshVertexBuffer;
      GPUBuffer m_bboxesBuffer;

      GLenum m_primitiveType;

      bool m_instanced;

      unsigned int m_instanceNumber;
      unsigned int m_meshNumber;
      GLuint m_vertexStride;

      unsigned int m_vboSize;

      unsigned int m_vertexOffset;
      unsigned int m_vertexCount;

      std::map<util::ResourceHandle, unsigned int, Less> m_instanceNumberPerMesh;

      ////////////////////////////////////////////////////////////////
    };
  }
}

#endif
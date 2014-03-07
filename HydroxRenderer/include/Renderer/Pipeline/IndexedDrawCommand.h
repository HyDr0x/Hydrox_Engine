#ifndef INDEXEDDRAWCOMMAND_H_
#define INDEXEDDRAWCOMMAND_H_

#include "Renderer/Pipeline/DrawCommandInterface.h"

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

    class IndexedDrawCommand : public DrawCommandInterface
    {
    public:

      IndexedDrawCommand(GLenum primitiveType, GLuint vertexStride, GLenum indexType);
      ~IndexedDrawCommand();

      void multiDraw();
      void createBuffer(unsigned int primCount, unsigned int vboSize, unsigned int iboSize);
      void fillCaches(unsigned int commandIndex, unsigned int instanceCount, Mesh *mesh);
      void fillBuffer();
      void updateCommandBuffer(unsigned int commandIndex, unsigned int instanceCount);

    private:

      std::vector<DrawElementsIndirectCommand> m_commandCache;

      GLenum m_indexType;

      //per mesh buffer
      GPUBuffer m_meshIndexBuffer;

      unsigned int m_indexCount;
      unsigned int m_indexOffset;
    };
  }
}

#endif
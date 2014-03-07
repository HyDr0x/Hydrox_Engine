#ifndef ARRAYDRAWCOMMAND_H_
#define ARRAYDRAWCOMMAND_H_

#include "Renderer/Pipeline/DrawCommandInterface.h"

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

    class ArrayDrawCommand : public DrawCommandInterface
    {
    public:

      ArrayDrawCommand(GLenum primitiveType, GLuint vertexStride);
      ~ArrayDrawCommand();

      void multiDraw();
      void createBuffer(unsigned int primCount, unsigned int vboSize, unsigned int iboSize);
      void fillCaches(unsigned int commandIndex, unsigned int instanceCount, Mesh *mesh);
      void fillBuffer();
      void updateCommandBuffer(unsigned int commandIndex, unsigned int instanceCount);

    private:

      std::vector<DrawArraysIndirectCommand> m_commandCache;
    };
  }
}

#endif
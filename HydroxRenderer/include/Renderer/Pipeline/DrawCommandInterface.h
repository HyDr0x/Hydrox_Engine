#ifndef DRAWCOMMANDINTERFACE_H_
#define DRAWCOMMANDINTERFACE_H_

#include <vector>

#include <Utilities/Math/Math.hpp>

#include "Renderer/Buffer/GPUBuffer.h"

namespace he
{
	namespace renderer
	{
    class Mesh;

    class DrawCommandInterface
    {
    public:

      DrawCommandInterface(GLenum primitiveType, GLuint vertexStride) : m_primitiveType(primitiveType), m_vertexStride(vertexStride)
      {
      }

      virtual void multiDraw() = 0;
      virtual void createBuffer(unsigned int primCount, unsigned int vboSize, unsigned int iboSize) = 0;
      virtual void fillCaches(unsigned int commandIndex, unsigned int instanceCount, Mesh *mesh) = 0;
      virtual void fillBuffer() = 0;
      virtual void updateCommandBuffer(unsigned int commandIndex, unsigned int instanceCount) = 0;

    protected:

      std::vector<util::Vector<unsigned int, 4>> m_cullingCommandCache;
      std::vector<util::Vector<float, 4>> m_boundingBoxCache;

      //per mesh buffer
      GPUBuffer m_commandBuffer;
      GPUBuffer m_meshVertexBuffer;
      GPUBuffer m_bboxesBuffer;
      GPUBuffer m_cullingCommandBuffer;

      GLenum m_primitiveType;

      unsigned int m_primitiveCount;
      GLuint m_vertexStride;

      unsigned int m_vertexOffset;
      unsigned int m_vertexCount;
      unsigned int m_instanceCounter;
    };
  }
}

#endif
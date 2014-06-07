#ifndef COMPUTESHADER_H_
#define COMPUTESHADER_H_

#include <GL/glew.h>

#include "Renderer/DLLExport.h"

#include "Renderer/Resources/Shader.h"

namespace he
{
  namespace renderer
  {
    class GRAPHICAPI ComputeShader : public Shader
    {
    public:

      ComputeShader();
      ComputeShader(std::string shaderName, std::string computeShaderSource);
      ComputeShader(const ComputeShader& other);

      ~ComputeShader();

      ComputeShader& operator=(const ComputeShader& other);

      static void dispatchComputeShader(GLuint workGroupNumberX, GLuint workGroupNumberY, GLuint workGroupNumberZ);
      static void dispatchComputeShaderIndirect(GLuint dispatchBuffer, GLuint offset);

      std::string getShaderSource() const;

    private:

      bool createProgram(std::string shaderName, GLuint computeShader);

      std::string m_shaderSource;
    };
  }
}

#endif

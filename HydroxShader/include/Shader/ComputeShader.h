#ifndef COMPUTESHADER_H_
#define COMPUTESHADER_H_

#include <GL/glew.h>

#include "Shader/DLLExport.h"

#include "Shader/Shader.h"

namespace he
{
  namespace sh
  {
    class GRAPHICAPI ComputeShader : public Shader
    {
    public:

      ComputeShader();
      ComputeShader(const std::string& shaderName, const std::vector<std::string>& shaderSourceNames, const std::vector<std::string>& computeShaderSource);
      ComputeShader(const ComputeShader& other);

      ~ComputeShader();

      ComputeShader& operator=(const ComputeShader& other);

      static void dispatchComputeShader(GLuint workGroupNumberX, GLuint workGroupNumberY, GLuint workGroupNumberZ);
      static void dispatchComputeShaderIndirect(GLuint dispatchBuffer, GLuint offset);

    private:

      bool createProgram(std::string shaderName, GLuint computeShader);
    };
  }
}

#endif

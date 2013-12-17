#ifndef COMPUTESHADER_H_
#define COMPUTESHADER_H_

#include <GL/glew.h>

#include "Hydrox/DLLExport.h"

#include "Hydrox/Graphics/Shader.h"

namespace he
{
  class GRAPHICAPI ComputeShader : public Shader
  {
  public:

    ComputeShader();
    ComputeShader(std::string shaderName, std::string computeShaderSource);
	  ComputeShader(const ComputeShader& o);
    ComputeShader& operator=(const ComputeShader& o);

	  ~ComputeShader();

    void dispatchComputeShader(GLuint workGroupNumberX, GLuint workGroupNumberY, GLuint workGroupNumberZ) const;
    void dispatchComputeShaderIndirect(GLuint dispatchBuffer, GLuint offset) const;

  private:

    bool createProgram(std::string shaderName, GLuint computeShader);
  };
}

#endif
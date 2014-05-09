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
	    ComputeShader(const ComputeShader& o);

	    ~ComputeShader();

      ComputeShader& operator=(const ComputeShader& o);

      static void dispatchComputeShader(GLuint workGroupNumberX, GLuint workGroupNumberY, GLuint workGroupNumberZ);
      static void dispatchComputeShaderIndirect(GLuint dispatchBuffer, GLuint offset);

    private:

      bool createProgram(std::string shaderName, GLuint computeShader);
    };
	}
}

#endif

#include "Renderer/Resources/ComputeShader.h"

namespace he
{
	namespace renderer
	{
    ComputeShader::ComputeShader()
    {
      m_program = 0;
    }
    ComputeShader::ComputeShader(const ComputeShader& o)
    {
      m_hash = o.m_hash;
      m_program = o.m_program;
      m_shaderSource = o.m_shaderSource;
    }

    ComputeShader::ComputeShader(std::string shaderName, std::string computeShaderSource)
    {
      m_hash = MurmurHash64A(computeShaderSource.c_str(), computeShaderSource.size(), 0);

      GLuint computeShader;
      computeShader = createShader(GL_COMPUTE_SHADER, shaderName, computeShaderSource);

      m_shaderSource = computeShaderSource;

      createProgram(shaderName, computeShader);
    }

    ComputeShader::~ComputeShader()
    {
    }

    ComputeShader& ComputeShader::operator=(const ComputeShader& o)
    {
      m_hash = o.m_hash;
	    m_program = o.m_program;
      m_shaderSource = o.m_shaderSource;

      return *this;
    }

    void ComputeShader::dispatchComputeShader(GLuint workGroupNumberX, GLuint workGroupNumberY, GLuint workGroupNumberZ)
    {
      glDispatchCompute(workGroupNumberX, workGroupNumberY, workGroupNumberZ);
    }

    void ComputeShader::dispatchComputeShaderIndirect(GLuint dispatchBuffer, GLuint offset)
    {
      glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, dispatchBuffer);
      glDispatchComputeIndirect(offset);
      glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, 0);
    }

    std::string ComputeShader::getShaderSource() const
    {
      return m_shaderSource;
    }

    bool ComputeShader::createProgram(std::string shaderName, GLuint computeShader)
    {
      m_program = glCreateProgram();


      glAttachShader(m_program, computeShader);
      glDeleteShader(computeShader);

      glLinkProgram(m_program);

      GLint errorCode;
      glGetProgramiv(m_program, GL_LINK_STATUS, &errorCode);

      if(errorCode == GL_FALSE)
      {
        GLsizei length;
        glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &length);

	      GLchar *errorLog = new GLchar[length];
	      glGetProgramInfoLog(m_program, length, nullptr, errorLog);

        std::cout << "Error linking shader program " << shaderName <<  " because of:/n "<< errorLog << std::endl;

        delete[] errorLog;
	      glDeleteProgram(m_program);

        return false;
      }

      return true;
    }
	}
}

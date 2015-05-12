#include "Shader/ComputeShader.h"

namespace he
{
  namespace sh
  {
    ComputeShader::ComputeShader()
    {
      m_program = 0;
    }
    ComputeShader::ComputeShader(const ComputeShader& other)
    {
      m_program = other.m_program;
      m_shaderSource = other.m_shaderSource;
      m_shaderHashes = other.m_shaderHashes;
    }

    ComputeShader::ComputeShader(std::string shaderName, std::string computeShaderSource)
    {
      GLuint computeShader;
      computeShader = createShader(GL_COMPUTE_SHADER, shaderName, computeShaderSource);

      m_shaderSource = computeShaderSource;

      m_shaderHashes.push_back(MurmurHash64A(m_shaderSource.c_str(), m_shaderSource.size(), 0));

      createProgram(shaderName, computeShader);
    }

    ComputeShader::~ComputeShader()
    {
    }

    ComputeShader& ComputeShader::operator=(const ComputeShader& other)
    {
      m_program = other.m_program;
      m_shaderSource = other.m_shaderSource;
      m_shaderHashes = other.m_shaderHashes;

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

        std::cerr << "Error linking shader program " << shaderName <<  " because of:/n "<< errorLog << std::endl;

        delete[] errorLog;
        glDeleteProgram(m_program);

        return false;
      }

      return true;
    }
  }
}

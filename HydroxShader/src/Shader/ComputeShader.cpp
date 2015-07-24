#include "Shader/ComputeShader.h"

namespace he
{
  namespace sh
  {
    ComputeShader::ComputeShader()
    {}

    ComputeShader::ComputeShader(const std::string& shaderName, const std::vector<std::string>& shaderSourceNames, const std::vector<std::string>& computeShaderSource) : 
      Shader(shaderName, shaderSourceNames, computeShaderSource)
    {
      GLuint computeShader;
      computeShader = createShader(GL_COMPUTE_SHADER, shaderName, m_shaderSources[0]);

      m_shaderHashes.push_back(MurmurHash64A(m_shaderSources[0].c_str(), m_shaderSources[0].size(), 0));

      createProgram(shaderName, computeShader);
    }

    ComputeShader::~ComputeShader()
    {
    }

    ComputeShader::ComputeShader(const ComputeShader& other)
    {
      m_program = other.m_program;
      m_shaderName = other.m_shaderName;
      m_shaderSourceNames = other.m_shaderSourceNames;
      m_shaderSources = other.m_shaderSources;
      m_shaderHashes = other.m_shaderHashes;
    }

    ComputeShader& ComputeShader::operator=(const ComputeShader& other)
    {
      m_program = other.m_program;
      m_shaderName = other.m_shaderName;
      m_shaderSourceNames = other.m_shaderSourceNames;
      m_shaderSources = other.m_shaderSources;
      m_shaderHashes = other.m_shaderHashes;

      return *this;
    }

    void ComputeShader::dispatchComputeShader(GLuint workGroupNumberX, GLuint workGroupNumberY, GLuint workGroupNumberZ)
    {
      glDispatchCompute(workGroupNumberX, workGroupNumberY, workGroupNumberZ);
    }

    void ComputeShader::dispatchComputeShaderIndirect(GLintptr offset)
    {
      glDispatchComputeIndirect(offset);
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

        std::clog << "Error linking shader program " << shaderName <<  " because of:/n "<< errorLog << std::endl;

        delete[] errorLog;
        glDeleteProgram(m_program);

        return false;
      }

      return true;
    }
  }
}

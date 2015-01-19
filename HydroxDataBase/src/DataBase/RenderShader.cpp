#include "DataBase/RenderShader.h"

namespace he
{
  namespace db
  {
    RenderShader::RenderShader()
    {
    }

    RenderShader::RenderShader(const std::string& shaderName,
                               const std::string& vertexShaderSource, 
                               const std::string& fragmentShaderSource, 
                               const std::string& geometryShaderSource, 
                               const std::string& tesselationCTRLShaderSource, 
                               const std::string& tesselationEVALShaderSource)
    {
      std::string data = std::string();
      data += vertexShaderSource + tesselationCTRLShaderSource + tesselationEVALShaderSource + geometryShaderSource + fragmentShaderSource;

      m_hash = MurmurHash64A(data.c_str(), data.size(), 0);

      GLuint vertexShader;
      GLuint tesselationControlShader;
      GLuint tesselationEvaluationShader;
      GLuint geometryShader; 
      GLuint fragmentShader;

      vertexShader = createShader(GL_VERTEX_SHADER, shaderName, vertexShaderSource);
      tesselationControlShader = createShader(GL_TESS_CONTROL_SHADER, shaderName, tesselationCTRLShaderSource);
      tesselationEvaluationShader = createShader(GL_TESS_EVALUATION_SHADER, shaderName, tesselationEVALShaderSource);
      geometryShader = createShader(GL_GEOMETRY_SHADER, shaderName, geometryShaderSource);
      fragmentShader = createShader(GL_FRAGMENT_SHADER, shaderName, fragmentShaderSource);

      m_shaderSources.push_back(vertexShaderSource);
      m_shaderSources.push_back(fragmentShaderSource);
      m_shaderSources.push_back(geometryShaderSource);
      m_shaderSources.push_back(tesselationCTRLShaderSource);
      m_shaderSources.push_back(tesselationEVALShaderSource);
      
      createProgram(shaderName, vertexShader, tesselationControlShader, tesselationEvaluationShader, geometryShader, fragmentShader);
    }

    RenderShader::RenderShader(const RenderShader& other)
    {
      m_hash = other.m_hash;
      m_program = other.m_program;
      m_shaderSources = other.m_shaderSources;
    }

    RenderShader::~RenderShader()
    {
    }

    RenderShader& RenderShader::operator=(const RenderShader& other)
    {
      m_hash = other.m_hash;
      m_program = other.m_program;
      m_shaderSources = other.m_shaderSources;

      return *this;
    }

    void RenderShader::enableTransformFeedback(int count, const GLchar** varyings, GLenum buffertype) const
    {
      glTransformFeedbackVaryings(m_program, count, varyings, buffertype);
      glLinkProgram(m_program);//new linking because some unused varyings could be in use now
  
      GLsizei length;
      GLsizei size;
      GLenum type;
  
      for(int i = 0; i != count; i++)
      {
        glGetTransformFeedbackVarying(m_program, i, 0, &length, nullptr, nullptr, nullptr);

        GLchar *var = new GLchar[length];

        glGetTransformFeedbackVarying(m_program, i, length, nullptr, &size, &type, var);

        delete[] var;
    
        if(strcmp(var, varyings[i]))
        {
          std::cout<<"Error, the TransformFeedback varying " << varyings[i] << "of type " << type << " and with size " << size << " is erroneous." << std::endl;
        }
      }
    }

    std::vector<std::string> RenderShader::getShaderSources() const
    {
      return m_shaderSources;
    }

    bool RenderShader::createProgram(std::string shaderName, GLuint vertexShader, 
                                                             GLuint tesselationControlShader, 
                                                             GLuint tesselationEvaluationShader, 
                                                             GLuint geometryShader, 
                                                             GLuint fragmentShader)
    {
      m_program = glCreateProgram();

      glAttachShader(m_program, vertexShader);
      glDeleteShader(vertexShader);

      if(tesselationControlShader != 0)
      {
        glAttachShader(m_program, tesselationControlShader);
        glDeleteShader(tesselationControlShader);
      }

      if(tesselationEvaluationShader != 0)
      {
        glAttachShader(m_program, tesselationEvaluationShader);
        glDeleteShader(tesselationEvaluationShader);
      }

      if(geometryShader != 0)
      {
        glAttachShader(m_program, geometryShader);
        glDeleteShader(geometryShader);
      }

      if(fragmentShader != 0)
      {
        glAttachShader(m_program, fragmentShader);
        glDeleteShader(fragmentShader);
      }

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

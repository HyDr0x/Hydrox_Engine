#include "DataBase/Shader.h"

namespace he
{
  namespace db
  {
  

    Shader::Shader()
    {
      m_program = 0;
    }

    Shader::~Shader()
    {
    }

    void Shader::free()
    {
      glDeleteProgram(m_program);
      m_program = 0;
    }

    void Shader::getUniformParameters(const char **uniformNames, const GLuint uniformCount, std::vector<GLuint> *uniformIndices, 
                                                                                            std::vector<GLint> *uniformOffsets, 
                                                                                            std::vector<GLint> *arrayStrides, 
                                                                                            std::vector<GLint> *matrixStrides) const
    {
      glGetUniformIndices  ( m_program, uniformCount, uniformNames, &(*uniformIndices)[0]);
      glGetActiveUniformsiv( m_program, uniformCount, &(*uniformIndices)[0], GL_UNIFORM_OFFSET,        &(*uniformOffsets)[0]);
      glGetActiveUniformsiv( m_program, uniformCount, &(*uniformIndices)[0], GL_UNIFORM_ARRAY_STRIDE,  &(*arrayStrides)[0]);
      glGetActiveUniformsiv( m_program, uniformCount, &(*uniformIndices)[0], GL_UNIFORM_MATRIX_STRIDE, &(*matrixStrides)[0]);//maybe useless?!
    }

    void Shader::setUniform(GLint location, GLint type, const GLfloat* val)
    {
      switch(type)
      {
      case GL_FLOAT:
        glUniform1f(location,val[0]);
        break;
      case GL_FLOAT_VEC2:
        glUniform2f(location,val[0],val[1]);
        break;
      case GL_FLOAT_VEC3:
        glUniform3f(location,val[0],val[1],val[2]);
        break;
      case GL_FLOAT_VEC4:
        glUniform4f(location,val[0],val[1],val[2],val[3]);
        break;
      case GL_FLOAT_MAT2:
        glUniformMatrix2fv(location,1,GL_FALSE,val);
        break;
      case GL_FLOAT_MAT3:
        glUniformMatrix3fv(location,1,GL_FALSE,val);
        break;
      case GL_FLOAT_MAT4:
        glUniformMatrix4fv(location,1,GL_FALSE,val);
        break;
      }
    }

    void Shader::setUniform(GLint location, GLint type, const GLuint* val)
    {
      switch(type)
      {
      case GL_UNSIGNED_INT:
        glUniform1ui(location,val[0]);
        break;
      case GL_UNSIGNED_INT_VEC2:
        glUniform2ui(location,val[0],val[1]);
        break;
      case GL_UNSIGNED_INT_VEC3:
        glUniform3ui(location,val[0],val[1],val[2]);
        break;
      case GL_UNSIGNED_INT_VEC4:
        glUniform4ui(location,val[0],val[1],val[2],val[3]);
        break;
      }
    }

    void Shader::setUniform(GLint location, GLint type, const GLint* val)
    {
      switch(type)
      {
      case GL_INT:
        glUniform1i(location,val[0]);
        break;
      case GL_INT_VEC2:
        glUniform2i(location,val[0],val[1]);
        break;
      case GL_INT_VEC3:
        glUniform3i(location,val[0],val[1],val[2]);
        break;
      case GL_INT_VEC4:
        glUniform4i(location,val[0],val[1],val[2],val[3]);
        break;
      }
    }

    void Shader::useShader() const
    {
      glUseProgram(m_program);
    }

    void Shader::useNoShader() const
    {
      glUseProgram(0);
    }

    GLuint Shader::createShader(GLenum shaderType, std::string shaderName, std::string shaderSource) const
    {
      if(shaderSource.empty())
      {
        return 0;
      }

      const GLchar *include = shaderSource.c_str();

      GLuint shader = glCreateShader(shaderType);
      glShaderSource(shader, 1, &include, nullptr );
      glCompileShader(shader);

      if(!checkShaderStatus(shader, shaderName))
      {
        glDeleteShader(shader);
        return 0;
      }

      return shader;
    }

    bool Shader::checkShaderStatus(GLuint shader, std::string shaderName) const
    {
      GLint errorCode;
      glGetShaderiv(shader, GL_COMPILE_STATUS, &errorCode);

      if(errorCode == GL_FALSE)
      {
        GLsizei length;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

        GLchar *errorLog = new GLchar[length];
        glGetShaderInfoLog(shader, length, nullptr, errorLog);

        std::cerr << "Error compiling " << shaderName << " because of " << errorLog << std::endl;

        delete[] errorLog;

        return false;
      }

      return true;
    }

  }
}

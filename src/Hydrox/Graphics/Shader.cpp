#include "Hydrox/Graphics/Shader.h"

namespace he
{
  Shader::Shader()
  {
    m_program = 0;
  }
  Shader::Shader(const Shader& o)
  {
    m_hash = o.m_hash;
    m_program = o.m_program;
  }

  Shader::Shader(std::string shaderName,
                 std::string vertexShaderSource, 
			           std::string fragmentShaderSource, 
			           std::string geometryShaderSource, 
			           std::string tesselationCTRLShaderSource, 
                 std::string tesselationEVALShaderSource,
                 std::string computeShaderSource)
  {
    std::string data = std::string();
    data += shaderName + vertexShaderSource + fragmentShaderSource + geometryShaderSource + tesselationCTRLShaderSource + tesselationEVALShaderSource + computeShaderSource;

    m_hash = MurmurHash64A(data.c_str(), data.size(), 0);

	  bool allOK = true;

    GLuint computeShader;
    allOK = createShader(GL_COMPUTE_SHADER, shaderName, computeShaderSource, computeShader);

    if(allOK)//compute shader
    {
      createProgram(m_program, shaderName, computeShader, 0, 0, 0, 0, 0);
    }

    GLuint vertexShader;
    GLuint tesselationControlShader;
    GLuint tesselationEvaluationShader;
    GLuint geometryShader; 
    GLuint fragmentShader;
  
    createShader(GL_VERTEX_SHADER, shaderName, vertexShaderSource, vertexShader);
    createShader(GL_TESS_CONTROL_SHADER, shaderName, tesselationCTRLShaderSource, tesselationControlShader);
    createShader(GL_TESS_EVALUATION_SHADER, shaderName, tesselationEVALShaderSource, tesselationEvaluationShader);
    createShader(GL_GEOMETRY_SHADER, shaderName, geometryShaderSource, geometryShader);
    createShader(GL_FRAGMENT_SHADER, shaderName, fragmentShaderSource, fragmentShader);

    createProgram(m_program, shaderName, 0, vertexShader, tesselationControlShader, tesselationEvaluationShader, geometryShader, fragmentShader);
  }

  Shader& Shader::operator=(const Shader& o)
  {
    if(m_program != 0)
    {
      glDeleteProgram(m_program);
    }

    m_hash = o.m_hash;
	  m_program = o.m_program;

    return *this;
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
                                                                                          std::vector<GLint> *matrixStrides)
  {
    glGetUniformIndices  ( m_program, uniformCount, uniformNames, &(*uniformIndices)[0]);
	  glGetActiveUniformsiv( m_program, uniformCount, &(*uniformIndices)[0], GL_UNIFORM_OFFSET,        &(*uniformOffsets)[0]);
	  glGetActiveUniformsiv( m_program, uniformCount, &(*uniformIndices)[0], GL_UNIFORM_ARRAY_STRIDE,  &(*arrayStrides)[0]);
	  glGetActiveUniformsiv( m_program, uniformCount, &(*uniformIndices)[0], GL_UNIFORM_MATRIX_STRIDE, &(*matrixStrides)[0]);//maybe useless?!
  }

  void Shader::setUniform(GLint location, GLint type, const GLfloat* val) const
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

  void Shader::setUniform(GLint location, GLint type, const GLuint* val) const
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

  void Shader::setUniform(GLint location, GLint type, const GLint* val) const
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

  void Shader::setTexture(GLint location, GLint slot) const
  {
	  glUniform1i(location, slot);
  }

  void Shader::enableTransformFeedback(int count, const GLchar** varyings, GLenum buffertype) const
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

  void Shader::dispatchComputeShader(GLuint workGroupNumberX, GLuint workGroupNumberY, GLuint workGroupNumberZ) const
  {
    glDispatchCompute(workGroupNumberX, workGroupNumberY, workGroupNumberZ);
  }

  void Shader::dispatchComputeShaderIndirect(GLuint dispatchBuffer, GLuint offset) const
  {
    glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, dispatchBuffer);
    glDispatchComputeIndirect(offset);
    glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, 0);
  }

  void Shader::useShader() const
  {
	  glUseProgram(m_program);
  }

  void Shader::useNoShader() const
  {
	  glUseProgram(0);
  }

  bool Shader::createProgram(GLuint& program, std::string shaderName, GLuint computeShader, 
                                                                      GLuint vertexShader, 
                                                                      GLuint tesselationControlShader, 
                                                                      GLuint tesselationEvaluationShader, 
                                                                      GLuint geometryShader, 
                                                                      GLuint fragmentShader)
  {
    program = glCreateProgram();

    if(vertexShader != 0)
    {
	    glAttachShader(m_program, vertexShader);
      glDeleteShader(vertexShader);

	    if(fragmentShader != 0)
      {
		    glAttachShader(m_program, fragmentShader);
        glDeleteShader(fragmentShader);
      }
	    if(geometryShader != 0)
      {
		    glAttachShader(m_program, geometryShader);
        glDeleteShader(geometryShader);
      }
	    if(tesselationControlShader != 0)
	    {
		    glAttachShader(m_program, tesselationControlShader);
		    glAttachShader(m_program, tesselationEvaluationShader);
        glDeleteShader(tesselationControlShader);
		    glDeleteShader(tesselationEvaluationShader);
	    }
    }
    else
    {
      glAttachShader(m_program, computeShader);
      glDeleteShader(computeShader);
    }

    glLinkProgram(program);

    GLint errorCode;
    glGetProgramiv(program, GL_LINK_STATUS, &errorCode);

    if(errorCode == GL_FALSE)
    {
      GLsizei length;
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

	    GLchar *errorLog = new GLchar[length];
	    glGetProgramInfoLog(program, length, nullptr, errorLog);

      std::cout << "Error linking shader program " << shaderName <<  " because of:\n "<< errorLog << std::endl;

      delete[] errorLog;
	    glDeleteProgram(program);

      return false;
    }

    return true;
  }

  bool Shader::createShader(GLenum shaderType, std::string shaderName, std::string shaderSource, GLuint& shader)
  {
    if(shaderSource.empty())
    {
      shader = 0;
      return false;
    }

    const GLchar *include = shaderSource.c_str();

    shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &include, nullptr );
    glCompileShader(shader);

    if(!checkShaderStatus(shader, shaderName))
	  {
		  glDeleteShader(shader);
      shader = 0;
      return false;
    }

    return true;
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

		  std::cout << "Error compiling " << shaderName << " because of " << errorLog << std::endl;

      delete[] errorLog;

		  return false;
	  }

	  return true;
  }
}

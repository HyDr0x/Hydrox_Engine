#include "Hydrox/Graphics/Shader.h"

namespace he
{
  Shader::Shader()
  {
    m_program = 0;
  }
  Shader::Shader(const Shader& o)
  {
    m_program = o.m_program;
  }

  Shader::Shader(const char *vertexFilename, 
			            const char *fragmentFilename, 
			            const char *geometryFilename, 
			            const char *tesselationCTRLFilename, 
                  const char *tesselationEVALFilename,
                  const char *computeFilename,
                  std::vector<std::string>& dynamicDefines)
  {
	  bool error = true;

    GLuint computeShader;
    error = createShader(GL_COMPUTE_SHADER, computeFilename, dynamicDefines, computeShader);

    if(error)//compute shader
    {
      createProgram(m_program, computeFilename, computeShader, 0, 0, 0, 0, 0);
    }

    GLuint vertexShader;
    GLuint tesselationControlShader;
    GLuint tesselationEvaluationShader;
    GLuint geometryShader; 
    GLuint fragmentShader;
  
    createShader(GL_VERTEX_SHADER, vertexFilename, dynamicDefines, vertexShader);
    createShader(GL_TESS_CONTROL_SHADER, tesselationCTRLFilename, dynamicDefines, tesselationControlShader);
    createShader(GL_TESS_EVALUATION_SHADER, tesselationEVALFilename, dynamicDefines, tesselationEvaluationShader);
    createShader(GL_GEOMETRY_SHADER, geometryFilename, dynamicDefines, geometryShader);
    createShader(GL_FRAGMENT_SHADER, fragmentFilename, dynamicDefines, fragmentShader);

    createProgram(m_program, vertexFilename, 0, vertexShader, tesselationControlShader, tesselationEvaluationShader, geometryShader, fragmentShader);
  }

  Shader& Shader::operator=(const Shader& o)
  {
    if(m_program != 0)
    {
      glDeleteProgram(m_program);
    }

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

  void Shader::enableTransformFeedback(int count, const char** varyings, GLenum buffertype) const
  {
	  glTransformFeedbackVaryings(m_program, count, varyings, buffertype);
	  glLinkProgram(m_program);//new linking because some unused varyings could be in use now
	
	  int length;
	  GLsizei size;
	  GLenum type;
	
	  for(int i = 0; i != count; i++)
	  {
      glGetTransformFeedbackVarying(m_program, i, 0, &length, nullptr, nullptr, nullptr);

      char *var = new char[length];

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

  bool Shader::createProgram(GLuint& program, const char *shaderName, GLuint computeShader, 
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

  bool Shader::createShader(GLenum shaderType, const char *shaderFileName, std::vector<std::string>& dynamicDefines, GLuint& shader)
  {
    if(shaderFileName == nullptr)
    {
      shader = 0;
      return false;
    }

	  std::string source;

    if(!loadShaderSource(shaderFileName, source, dynamicDefines))
    {
      shader = 0;
      return false;
    }

    const char *include = source.c_str();

    shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &include, nullptr );
    glCompileShader(shader);

    if(!checkShaderStatus(shader, shaderFileName))
	  {
		  glDeleteShader(shader);
      shader = 0;
      return false;
    }

    return true;
  }

  bool Shader::loadShaderSource(const char *filename, std::string &shadersource, std::vector<std::string> &dynamicDefines)
  {
	  std::ifstream file(filename);
	  std::string line;

	  if(file.is_open())
    {
		  while(!file.eof())
		  {
			  std::getline(file, line);
			  line += '\n';

        for(int i = 0; i < dynamicDefines.size(); i++)
        {
          size_t pos = line.find(dynamicDefines[i]);
          if(pos != std::string::npos)
          {
            size_t defineOffset = dynamicDefines[i].find(" ");
            line.insert(pos + defineOffset, dynamicDefines[i], defineOffset, std::string::npos);
            dynamicDefines.erase(dynamicDefines.begin() + i);
          }
        }

			  shadersource += line;
		  }
    }
	  else
	  {
		  file.close();

		  std::cout << "Error: couldn't open shader source file " << filename << "." << std::endl;

		  return false;
	  }

	  file.close();
	  return true;
  }

  bool Shader::checkShaderStatus(GLuint shader, const char *filename) const
  {
	  GLint errorCode;
	  glGetShaderiv(shader, GL_COMPILE_STATUS, &errorCode);

	  if(errorCode == GL_FALSE)
	  {
      GLsizei length;

		  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

      GLchar *errorLog = new char[length];
      glGetShaderInfoLog(shader, length, nullptr, errorLog);

		  std::cout << "Error compiling " << filename << " because of " << errorLog << std::endl;

      delete[] errorLog;

		  return false;
	  }

	  return true;
  }
}

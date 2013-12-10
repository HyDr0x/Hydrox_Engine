#ifndef SHADER_H_
#define SHADER_H_

#include <iostream>
#include <string>
#include <fstream>
#include <assert.h>
#include <vector>

#include <GL/glew.h>

#include "Hydrox/DLLExport.h"

#include "Hydrox/Utility/Miscellaneous/CachedResource.h"
#include "Hydrox/Utility/Miscellaneous/ResourceHandle.h"

#include "Hydrox/Utility/Math/Math.hpp"

namespace he
{
  class GRAPHICAPI Shader : public CachedResource
  {
  public:

    enum ShaderSlots
	  {
		  POSITION,
		  TEXTURE0,
		  TEXTURE1,
		  TEXTURE2,
		  TEXTURE3,
		  NORMAL,
		  BINORMAL,
		  TANGENT,
		  BONEWEIGHTS,
		  BONEINDICES,
		  SPECIAL0,
		  SPECIAL1,
		  SPECIAL2,
		  SPECIAL3
	  };

    Shader();

	  Shader(const Shader& o);

    Shader(const char *vertexFilename, 
			      const char *fragmentFilename, 
			      const char *geometryFilename, 
			      const char *tesselationCTRLFilename, 
            const char *tesselationEVALFilename,
            const char *computeFilename,
            std::vector<std::string> &dynamicDefines = std::vector<std::string>());

    Shader& operator=(const Shader& o);

	  ~Shader();

    void free();

    void getUniformParameters(const char **uniformNames, const GLuint uniformCount, std::vector<GLuint> *uniformIndices, 
                                                                                    std::vector<GLint> *uniformOffsets, 
                                                                                    std::vector<GLint> *arrayStrides, 
                                                                                    std::vector<GLint> *matrixStrides);

	  void setUniform(GLint location, int type, const GLfloat* val) const;
    void setUniform(GLint location, int type, const GLuint* val) const;
	  void setUniform(GLint location, int type, const GLint* val) const;
	  void setTexture(GLint location, GLint slot) const;

	  void enableTransformFeedback(int count, const char** varyings, GLenum buffertype) const;
    void dispatchComputeShader(GLuint workGroupNumberX, GLuint workGroupNumberY, GLuint workGroupNumberZ) const;
    void dispatchComputeShaderIndirect(GLuint dispatchBuffer, GLuint offset) const;

	  void useShader() const;
	  void useNoShader() const;

  private:

    bool createProgram(GLuint& program, const char *shaderName, GLuint computeShader, 
                                                                GLuint vertexShader, 
                                                                GLuint tesselationControlShader, 
                                                                GLuint tesselationEvaluationShader, 
                                                                GLuint geometryShader, 
                                                                GLuint fragmentShader);
    bool createShader(GLenum shaderType, const char *shaderFileName, std::vector<std::string>& dynamicDefines, GLuint& shader);
	  bool loadShaderSource(const char *filename, std::string& shadersource, std::vector<std::string>& dynamicDefines);
	  bool checkShaderStatus(GLuint shader, const char *filename) const;

	  GLuint m_program;
  };
}

#endif
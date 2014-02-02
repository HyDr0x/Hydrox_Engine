#ifndef SHADER_H_
#define SHADER_H_

#include <iostream>
#include <string>
#include <assert.h>
#include <vector>

#include <GL/glew.h>

#include "Renderer/DLLExport.h"

#include "Renderer/Resources/ManagedResource.h"
#include <Utilities/Miscellaneous/ResourceHandle.h>

#include <Utilities/Math/Math.hpp>

namespace he
{
	namespace renderer
	{
	

    class GRAPHICAPI Shader : public ManagedResource
    {
    public:

      Shader();
	    virtual ~Shader() = 0;

      void free();

      void getUniformParameters(const char **uniformNames, const GLuint uniformCount, std::vector<GLuint> *uniformIndices, 
                                                                                      std::vector<GLint> *uniformOffsets, 
                                                                                      std::vector<GLint> *arrayStrides, 
                                                                                      std::vector<GLint> *matrixStrides);

	    void setUniform(GLint location, int type, const GLfloat* val) const;
      void setUniform(GLint location, int type, const GLuint* val) const;
	    void setUniform(GLint location, int type, const GLint* val) const;
	    void setTexture(GLint location, GLint slot) const;

	    void useShader() const;
	    void useNoShader() const;

    protected:

      bool createShader(GLenum shaderType, std::string shaderName, std::string shaderSource, GLuint& shader);
	    bool checkShaderStatus(GLuint shader, std::string shaderName) const;

	    GLuint m_program;
    };

	}
}

#endif

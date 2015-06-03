#ifndef SHADER_H_
#define SHADER_H_

#include <iostream>
#include <string>
#include <cassert>
#include <vector>

#include <GL/glew.h>

#include <Utilities/Miscellaneous/MurMurHash3.h>
#include <Utilities/Miscellaneous/ResourceHandle.h>

#include <Utilities/Math/Math.hpp>

#include "Shader/DLLExport.h"

namespace he
{
  namespace sh
  {
    class GRAPHICAPI Shader
    {
    public:

      Shader();
      Shader(const std::string& shaderName, const std::vector<std::string>& shaderSourceNames, const std::vector<std::string>& shaderSources);
      virtual ~Shader() = 0;

      bool operator==(const Shader& other) const;

      void free();

      void getUniformParameters(const char **uniformNames, const GLuint uniformCount, std::vector<GLuint> *uniformIndices,
        std::vector<GLint> *uniformOffsets,
        std::vector<GLint> *arrayStrides,
        std::vector<GLint> *matrixStrides) const;

      static void setUniform(GLint location, int type, const GLfloat* val);
      static void setUniform(GLint location, int type, const GLuint* val);
      static void setUniform(GLint location, int type, const GLint* val);

      void useShader() const;
      void useNoShader() const;

      std::string getShaderName() const;
      std::vector<std::string> getShaderSourceNames() const;
      std::vector<std::string> getShaderSources() const;

    protected:

      GLuint createShader(GLenum shaderType, std::string shaderName, std::string shaderSource) const;
      bool checkShaderStatus(GLuint shader, GLenum shaderType, std::string shaderName) const;

      std::string m_shaderName;
      std::vector<std::string> m_shaderSourceNames;
      std::vector<std::string> m_shaderSources;
      std::vector<uint64_t> m_shaderHashes;

      GLuint m_program;
    };
  }
}

#endif

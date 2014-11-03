#ifndef RENDERSHADER_H_
#define RENDERSHADER_H_

#include <GL/glew.h>

#include "DataBase/DLLExport.h"

#include "DataBase/Shader.h"

namespace he
{
  namespace db
  {
    class GRAPHICAPI RenderShader : public Shader
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
        COLOR,
        CACHEINDIZES0,
        CACHEINDIZES1,
        SPECIAL0,
        SPECIAL1,
        SPECIAL2,
        VERTEXDECLARATIONFLAGNUMBER,
      };

      RenderShader();
      RenderShader(const std::string& shaderName,
                   const std::string& vertexShaderSource, 
                   const std::string& fragmentShaderSource = std::string(), 
                   const std::string& geometryShaderSource = std::string(), 
                   const std::string& tesselationCTRLShaderSource = std::string(), 
                   const std::string& tesselationEVALShaderSource = std::string());
      RenderShader(const RenderShader& other);

      ~RenderShader();

      RenderShader& operator=(const RenderShader& other);

      void enableTransformFeedback(int count, const GLchar** varyings, GLenum buffertype) const;

      std::vector<std::string> getShaderSources() const;

    private:

      bool createProgram(std::string shaderName, GLuint vertexShader, 
                                                 GLuint tesselationControlShader, 
                                                 GLuint tesselationEvaluationShader, 
                                                 GLuint geometryShader, 
                                                 GLuint fragmentShader);

      std::vector<std::string> m_shaderSources;
    };

  }
}

#endif

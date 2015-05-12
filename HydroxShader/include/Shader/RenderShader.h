#ifndef RENDERSHADER_H_
#define RENDERSHADER_H_

#include <GL/glew.h>

#include "Shader/DLLExport.h"

#include "Shader/Shader.h"

#include <Utilities/Miscellaneous/Flags.hpp>

namespace he
{
  namespace sh
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
        BONEWEIGHTS,
        BONEINDICES,
        COLOR,
        CACHEINDICES0,
        CACHEINDICES1,
        SPECIAL0,
        SPECIAL1,
        SPECIAL2,
        VERTEXDECLARATIONFLAGNUMBER,
      };

      RenderShader();
      RenderShader(util::Flags<ShaderSlots> vertexDecaration,
                   const std::string& shaderName,
                   const std::string& vertexShaderSource, 
                   const std::string& fragmentShaderSource = std::string(), 
                   const std::string& geometryShaderSource = std::string(), 
                   const std::string& tesselationCTRLShaderSource = std::string(), 
                   const std::string& tesselationEVALShaderSource = std::string());

      RenderShader(const RenderShader& other);

      ~RenderShader();

      RenderShader& operator=(const RenderShader& other);

      bool sameShaderStage(const RenderShader& other) const;

      void enableTransformFeedback(int count, const GLchar** varyings, GLenum buffertype) const;

      std::vector<std::string> getShaderSources() const;

      util::Flags<ShaderSlots> getVertexDeclaration() const;

    private:

      bool createProgram(std::string shaderName, GLuint vertexShader, 
                                                 GLuint tesselationControlShader, 
                                                 GLuint tesselationEvaluationShader, 
                                                 GLuint geometryShader, 
                                                 GLuint fragmentShader);

      std::vector<std::string> m_shaderSources;
      
      util::Flags<ShaderSlots> m_vertexDecaration;
    };

    typedef util::Flags<RenderShader::ShaderSlots> ShaderSlotFlags;
  }
}

#endif

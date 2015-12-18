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
        SPECIAL0,
        SPECIAL1,
        SPECIAL2,
        VERTEXDECLARATIONFLAGNUMBER,
      };

      RenderShader();
      RenderShader(util::Flags<ShaderSlots> vertexDecaration,
                   const std::string& shaderName,
                   const std::vector<std::string>& shaderSourceNames,
                   const std::vector<std::string>& shaderSources);

      RenderShader(const RenderShader& other);

      ~RenderShader();

      RenderShader& operator=(const RenderShader& other);

      bool sameShaderStage(const RenderShader& other) const;

      void enableTransformFeedback(int count, const GLchar** varyings, GLenum buffertype) const;

      util::Flags<ShaderSlots> getVertexDeclaration() const;

    private:

      bool createProgram(std::string shaderName, GLuint vertexShader, 
                                                 GLuint tesselationControlShader, 
                                                 GLuint tesselationEvaluationShader, 
                                                 GLuint geometryShader, 
                                                 GLuint fragmentShader);
      
      util::Flags<ShaderSlots> m_vertexDecaration;
    };

    typedef util::Flags<RenderShader::ShaderSlots> ShaderSlotFlags;
  }
}

#endif

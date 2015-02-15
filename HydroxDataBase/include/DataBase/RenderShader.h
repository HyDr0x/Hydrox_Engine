#ifndef RENDERSHADER_H_
#define RENDERSHADER_H_

#include <GL/glew.h>

#include "DataBase/DLLExport.h"

#include "DataBase/Shader.h"
#include "DataBase/Mesh.h"

#include <Utilities/Miscellaneous/Flags.hpp>

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
      RenderShader(util::Flags<VertexElements> vertexDecaration,
                   const std::string& shaderName,
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

      util::Flags<VertexElements> getVertexDeclaration();

    private:

      bool createProgram(std::string shaderName, GLuint vertexShader, 
                                                 GLuint tesselationControlShader, 
                                                 GLuint tesselationEvaluationShader, 
                                                 GLuint geometryShader, 
                                                 GLuint fragmentShader);

      std::vector<std::string> m_shaderSources;
      util::Flags<VertexElements> m_vertexDecaration;
    };

  }
}

#endif

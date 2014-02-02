#ifndef RENDERSHADER_H_
#define RENDERSHADER_H_

#include <GL/glew.h>

#include "Renderer/DLLExport.h"

#include "Renderer/Resources/Shader.h"

namespace he
{
	namespace renderer
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
		    SPECIAL0,
		    SPECIAL1,
		    SPECIAL2,
		    SPECIAL3
	    };

      RenderShader();
      RenderShader(std::string shaderName,
                   std::string vertexShaderSource, 
			             std::string fragmentShaderSource = std::string(), 
			             std::string geometryShaderSource = std::string(), 
			             std::string tesselationCTRLShaderSource = std::string(), 
                   std::string tesselationEVALShaderSource = std::string());
	    RenderShader(const RenderShader& o);
      RenderShader& operator=(const RenderShader& o);

	    ~RenderShader();

	    void enableTransformFeedback(int count, const GLchar** varyings, GLenum buffertype) const;

    private:

      bool createProgram(std::string shaderName, GLuint vertexShader, 
                                                 GLuint tesselationControlShader, 
                                                 GLuint tesselationEvaluationShader, 
                                                 GLuint geometryShader, 
                                                 GLuint fragmentShader);
    };

	}
}

#endif

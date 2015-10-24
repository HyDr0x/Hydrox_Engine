#version 440 core

#include "../../HydroxShader/include/Shader/Shaderincludes/VertexDeclaration.glslh"

layout(location = SPECIAL1) in float in_placeHolder;

void main()
{
	gl_Position = vec4(0, 0, 0, 1);
}
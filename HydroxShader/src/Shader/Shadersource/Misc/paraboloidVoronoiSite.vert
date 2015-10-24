#version 440 core

#include "../../HydroxShader/include/Shader/Shaderincludes/VertexDeclaration.glslh"

layout(location = POSITION) in vec4 in_Pos;

out flat uint vsout_vertexIndex;

void main()
{
	vsout_vertexIndex = gl_VertexID;
	gl_Position = in_Pos;
}
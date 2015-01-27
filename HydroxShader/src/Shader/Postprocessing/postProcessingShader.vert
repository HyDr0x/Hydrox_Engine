#version 440 core

#include "../../../include/Shader/VertexDeclaration.glslh"

layout(location = SPECIAL1) flat in float in_placeHolder;

void main()
{
	gl_Position = vec4(0, 0, 0, 1);
}
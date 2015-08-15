#version 440 core

#include "../../../../include/Shader/Shaderincludes/VertexDeclaration.glslh"

layout(location = POSITION) in vec4 in_pos;
layout(location = NORMAL) in vec4 in_normalMat;

out vec4 vsout_normalMat;
out flat uint vsout_vertexID;

void main()
{
	vsout_normalMat = in_normalMat;
	vsout_vertexID = gl_VertexID;
	gl_Position = in_pos;
}
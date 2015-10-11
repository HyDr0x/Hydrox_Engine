#version 440 core

#include "../../../../include/Shader/Shaderincludes/VertexDeclaration.glslh"
#include "../../../../include/Shader/Shaderincludes/CameraUBO.glslh"

layout(location = POSITION) in vec4 in_pos;

out float vsout_cacheOrVertex;

void main()
{
	vsout_cacheOrVertex = in_pos.w;
	gl_Position = viewProjectionMatrix * vec4(in_pos.xyz, 1.0);
}
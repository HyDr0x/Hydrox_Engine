#version 440 core

#include "../../../../include/Shader/Shaderincludes/VertexDeclaration.glslh"
#include "../../../../include/Shader/Shaderincludes/CameraUBO.glslh"

layout(location = POSITION) in vec4 in_pos;

out vec3 vsout_normalMat;

void main()
{
	gl_Position = viewProjectionMatrix * vec4(in_pos.xyz, 1.0);
}
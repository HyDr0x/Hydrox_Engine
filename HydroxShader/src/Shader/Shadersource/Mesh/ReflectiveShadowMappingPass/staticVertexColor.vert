#version 440 core

#extension ARB_shader_draw_parameters : enable

#include "../../HydroxShader/include/Shader/Shaderincludes/VertexDeclaration.glslh"

layout(std430, binding = 0) buffer transformMatrixBuffer
{
	mat4 trfMatrix[];
};

layout(location = POSITION) in vec3 in_Pos;
layout(location = NORMAL) in vec3 in_normal;
layout(location = COLOR) in vec4 in_color;

out VertexData
{
	vec3 normal;
	vec4 color;
	flat uint instanceIndex;
} outData;

void main()
{
	outData.instanceIndex = uint(gl_InstanceID + gl_BaseInstanceARB);
	outData.color = in_color;
	
	outData.normal = normalize(mat3(trfMatrix[outData.instanceIndex]) * in_normal);
	gl_Position = trfMatrix[outData.instanceIndex] * vec4(in_Pos, 1.0);
}
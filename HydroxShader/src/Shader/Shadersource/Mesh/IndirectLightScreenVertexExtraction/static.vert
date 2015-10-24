#version 440 core

#extension GL_ARB_shader_draw_parameters : enable

#include "../../HydroxShader/include/Shader/Shaderincludes/VertexDeclaration.glslh"
#include "../../HydroxShader/include/Shader/Shaderincludes/CameraUBO.glslh"

layout(std430, binding = 0) buffer transformMatrixBuffer
{
	mat4 trfMatrix[];
};

layout(location = POSITION) in vec3 in_pos;
layout(location = NORMAL) in vec3 in_normal;

out VertexData
{
	vec3 pos3D;
	vec3 normal;
} outData;

void main()
{
	uint instanceIndex = uint(gl_InstanceID + gl_BaseInstanceARB);

	outData.pos3D = (trfMatrix[instanceIndex] * vec4(in_pos, 1.0)).xyz;
	outData.normal = normalize(mat3(trfMatrix[instanceIndex]) * in_normal);
	
	gl_Position = viewProjectionMatrix * vec4(outData.pos3D, 1.0);
}
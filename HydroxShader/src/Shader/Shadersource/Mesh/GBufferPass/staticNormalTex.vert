#version 440 core

#extension GL_ARB_shader_draw_parameters : enable

#include "../../HydroxShader/include/Shader/Shaderincludes/VertexDeclaration.glslh"
#include "../../HydroxShader/include/Shader/Shaderincludes/CameraUBO.glslh"

layout(std430, binding = 0) buffer transformMatrixBuffer
{
	mat4 trfMatrix[];
};

layout(location = POSITION) in vec3 in_Pos;
layout(location = TEXTURE0) in vec2 in_texCoord;
layout(location = NORMAL) in vec3 in_normal;
layout(location = BINORMAL) in vec3 in_binormal;

out VertexData
{
	vec2 texCoord;
	mat3 tangentToWorld;
	flat uint instanceIndex;
} outData;

void main()
{
	outData.instanceIndex = uint(gl_InstanceID + gl_BaseInstanceARB);
	
	vec3 tangent = cross(in_normal, in_binormal);
	
	mat3 normalWorld = mat3(trfMatrix[outData.instanceIndex]);
	outData.tangentToWorld[0] = normalWorld * tangent;
	outData.tangentToWorld[1] = normalWorld * in_binormal;
	outData.tangentToWorld[2] = normalWorld * in_normal;
	
	outData.texCoord = in_texCoord;

	gl_Position = viewProjectionMatrix * trfMatrix[outData.instanceIndex] * vec4(in_Pos, 1.0);
}
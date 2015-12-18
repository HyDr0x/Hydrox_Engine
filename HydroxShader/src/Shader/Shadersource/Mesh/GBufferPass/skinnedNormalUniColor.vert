#version 440 core

#extension ARB_shader_draw_parameters : enable

#define MAXBONES 64

#include "../../HydroxShader/include/Shader/Shaderincludes/VertexDeclaration.glslh"

layout(std430, binding = 0) buffer boneMatrixBuffer
{
	mat4 boneMatrix[];
};

layout(location = POSITION) in vec3 in_Pos;
layout(location = TEXTURE0) in vec2 in_texCoord;
layout(location = NORMAL) in vec3 in_normal;
layout(location = BINORMAL) in vec3 in_binormal;
layout(location = BONEWEIGHTS) in vec4 in_boneWeights;
layout(location = BONEINDICES) in vec4 in_boneIndices;

out VertexData
{
	vec2 texCoord;
	mat3 tangentToWorld;
	flat uint instanceIndex;
} outData;

void main()
{
	mat4 skinningMatrix = mat4(0.0f);

	outData.instanceIndex = gl_InstanceID + gl_BaseInstanceARB;
	uint globalInstanceID = MAXBONES * outData.instanceIndex;
	
	skinningMatrix += (boneMatrix[globalInstanceID + uint(in_boneIndices.x)] * in_boneWeights.x);
	skinningMatrix += (boneMatrix[globalInstanceID + uint(in_boneIndices.y)] * in_boneWeights.y);
	skinningMatrix += (boneMatrix[globalInstanceID + uint(in_boneIndices.z)] * in_boneWeights.z);
	skinningMatrix += (boneMatrix[globalInstanceID + uint(in_boneIndices.w)] * in_boneWeights.w);

	vec3 tangent = cross(in_normal, in_binormal);
	
	mat3 normalWorld = mat3(skinningMatrix);
	outData.tangentToWorld[0] = normalWorld * tangent;
	outData.tangentToWorld[1] = normalWorld * in_binormal;
	outData.tangentToWorld[2] = normalWorld * in_normal;
	
	outData.texCoord = in_texCoord;
	float scale = sqrt(skinningMatrix[0][0] * skinningMatrix[0][0] + skinningMatrix[0][1] * skinningMatrix[0][1] + skinningMatrix[0][2] * skinningMatrix[0][2]);
	
	gl_Position = vec4((skinningMatrix * vec4(in_Pos, 1.0)).xyz, scale);
}
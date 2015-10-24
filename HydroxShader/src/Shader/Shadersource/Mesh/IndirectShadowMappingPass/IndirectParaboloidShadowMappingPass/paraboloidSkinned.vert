#version 440 core

#extension ARB_shader_draw_parameters : enable

#include "../../HydroxShader/include/Shader/Shaderincludes/VertexDeclaration.glslh"

#define MAXBONES 64

layout(std430, binding = 0) buffer boneMatrixBuffer
{
	mat4 boneMatrix[];
};

out vec3 vsout_normal;

layout(location = POSITION) in vec4 in_Pos;
layout(location = NORMAL) in vec3 in_normal;
layout(location = BONEWEIGHTS) in vec4 in_boneWeights;
layout(location = BONEINDICES) in vec4 in_boneIndices;

void main()
{
	mat4 skinningMatrix = mat4(0.0f);

	uint globalInstanceID = MAXBONES * uint(gl_InstanceID + gl_BaseInstanceARB);
	
	skinningMatrix += (boneMatrix[globalInstanceID + uint(in_boneIndices.x)] * in_boneWeights.x);
	skinningMatrix += (boneMatrix[globalInstanceID + uint(in_boneIndices.y)] * in_boneWeights.y);
	skinningMatrix += (boneMatrix[globalInstanceID + uint(in_boneIndices.z)] * in_boneWeights.z);
	skinningMatrix += (boneMatrix[globalInstanceID + uint(in_boneIndices.w)] * in_boneWeights.w);

	vsout_normal = normalize(mat3(skinningMatrix) * in_normal);
	
	gl_Position = skinningMatrix * vec4(in_Pos.xyz, 1.0);
}
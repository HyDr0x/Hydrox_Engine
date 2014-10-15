#version 440 core

#extension ARB_shader_draw_parameters : enable

#include "../../Header/LightData.glslh"

#define MAXBONES 64

layout(std430, binding = 0) buffer boneMatrixBuffer
{
	mat4 boneMatrix[];
};

layout(std430, binding = 4) buffer shadowLightBuffer
{
	ShadowLightData shadowLight[];
};

layout(location = 2) uniform int lightIndex;

layout(location = 0) in vec3 in_Pos;
layout(location = 8) in vec4 in_boneWeights;
layout(location = 9) in vec4 in_boneIndices;

void main()
{
	mat4 skinningMatrix = mat4(0.0f);

	uint globalInstanceID = MAXBONES * uint(gl_InstanceID + gl_BaseInstanceARB);
	
	skinningMatrix += (boneMatrix[globalInstanceID + uint(in_boneIndices.x)] * in_boneWeights.x);
	skinningMatrix += (boneMatrix[globalInstanceID + uint(in_boneIndices.y)] * in_boneWeights.y);
	skinningMatrix += (boneMatrix[globalInstanceID + uint(in_boneIndices.z)] * in_boneWeights.z);
	skinningMatrix += (boneMatrix[globalInstanceID + uint(in_boneIndices.w)] * in_boneWeights.w);
	
	mat4 MVP = shadowLight[lightIndex].lightViewProj * skinningMatrix;
	
	gl_Position = MVP * vec4(in_Pos, 1.0f);
}
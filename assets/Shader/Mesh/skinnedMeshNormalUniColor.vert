#version 440 core

#extension ARB_shader_draw_parameters : enable

#define MAXBONES 64

#include "../Header/CameraUBO.glslh"

layout(std430, binding = 0) buffer boneMatrixBuffer
{
	mat4 boneMatrix[];
};

layout(location = 0) in vec3 in_Pos;
layout(location = 1) in vec2 in_texCoord;
layout(location = 5) in vec3 in_normal;
layout(location = 6) in vec3 in_binormal;
layout(location = 8) in vec4 in_boneWeights;
layout(location = 9) in vec4 in_boneIndices;

out vec2 vsout_texCoord;
out mat3 vsout_tangentToWorld;
flat out uint vsout_instanceIndex;

void main()
{
	mat4 skinningMatrix = mat4(0.0f);

	vsout_instanceIndex = gl_InstanceID + gl_BaseInstanceARB;
	uint globalInstanceID = MAXBONES * vsout_instanceIndex;
	
	skinningMatrix += (boneMatrix[globalInstanceID + uint(in_boneIndices.x)] * in_boneWeights.x);
	skinningMatrix += (boneMatrix[globalInstanceID + uint(in_boneIndices.y)] * in_boneWeights.y);
	skinningMatrix += (boneMatrix[globalInstanceID + uint(in_boneIndices.z)] * in_boneWeights.z);
	skinningMatrix += (boneMatrix[globalInstanceID + uint(in_boneIndices.w)] * in_boneWeights.w);

	vec3 tangent = cross(in_normal, in_binormal);
	
	mat3 normalWorld = mat3(skinningMatrix);
	vsout_tangentToWorld[0] = normalWorld * in_binormal;
	vsout_tangentToWorld[1] = normalWorld * in_normal;
	vsout_tangentToWorld[2] = normalWorld * tangent;
	
	vsout_texCoord = in_texCoord;
	gl_Position = viewProjectionMatrix * skinningMatrix * vec4(in_Pos, 1.0f);
}
#version 440 core

#extension ARB_shader_draw_parameters : enable

#include "../../../../../include/Shader/Shaderincludes/VertexDeclaration.glslh"
#include "../../../../../include/Shader/Shaderincludes/LightData.glslh"

#define MAXBONES 64

layout(std430, binding = 0) buffer boneMatrixBuffer
{
	mat4 boneMatrix[];
};

layout(std430, binding = 4) buffer reflectiveShadowLightBuffer
{
	ShadowLightData reflectiveShadowLight[];
};

layout(location = 2) uniform int lightIndex;

layout(location = POSITION) in vec3 in_Pos;
layout(location = TEXTURE0) in vec2 in_texCoord;
layout(location = NORMAL) in vec3 in_normal;
layout(location = BONEWEIGHTS) in vec4 in_boneWeights;
layout(location = BONEINDICES) in vec4 in_boneIndices;

out vec4 vsout_pos;
out vec2 vsout_texCoord;
out vec3 vsout_normal;
flat out uint vsout_instanceIndex;

void main()
{
	mat4 skinningMatrix = mat4(0.0f);

	vsout_instanceIndex = uint(gl_InstanceID + gl_BaseInstanceARB);
	uint globalInstanceID = MAXBONES * vsout_instanceIndex;
	
	skinningMatrix += (boneMatrix[globalInstanceID + uint(in_boneIndices.x)] * in_boneWeights.x);
	skinningMatrix += (boneMatrix[globalInstanceID + uint(in_boneIndices.y)] * in_boneWeights.y);
	skinningMatrix += (boneMatrix[globalInstanceID + uint(in_boneIndices.z)] * in_boneWeights.z);
	skinningMatrix += (boneMatrix[globalInstanceID + uint(in_boneIndices.w)] * in_boneWeights.w);
	
	vsout_pos = skinningMatrix * vec4(in_Pos, 1.0f);
	vsout_texCoord = in_texCoord;
	vsout_normal = normalize(mat3(skinningMatrix) * in_normal);
	gl_Position = reflectiveShadowLight[lightIndex].lightViewProj * vsout_pos;
}
#version 440 core

#extension ARB_shader_draw_parameters : enable

#define MAXBONES 64

#include "../../../../../include/Shader/Shaderincludes/VertexDeclaration.glslh"
#include "../../../../../include/Shader/Shaderincludes/CameraUBO.glslh"

layout(std430, binding = 0) buffer boneMatrixBuffer
{
	mat4 boneMatrix[];
};

layout(location = POSITION) in vec3 in_Pos;
layout(location = TEXTURE0) in vec2 in_texCoord;
layout(location = BONEWEIGHTS) in vec4 in_boneWeights;
layout(location = BONEINDICES) in vec4 in_boneIndices;

out vec2 vsout_texCoord;

void main()
{
	uint instanceIndex = uint(gl_InstanceID + gl_BaseInstanceARB);

	mat4 skinningMatrix = mat4(0.0f);

	uint globalInstanceID = MAXBONES * instanceIndex;
	
	skinningMatrix += (boneMatrix[globalInstanceID + uint(in_boneIndices.x)] * in_boneWeights.x);
	skinningMatrix += (boneMatrix[globalInstanceID + uint(in_boneIndices.y)] * in_boneWeights.y);
	skinningMatrix += (boneMatrix[globalInstanceID + uint(in_boneIndices.z)] * in_boneWeights.z);
	skinningMatrix += (boneMatrix[globalInstanceID + uint(in_boneIndices.w)] * in_boneWeights.w);

	vsout_texCoord = in_texCoord;
	
	gl_Position = viewProjectionMatrix * skinningMatrix * vec4(in_Pos, 1.0f);
}
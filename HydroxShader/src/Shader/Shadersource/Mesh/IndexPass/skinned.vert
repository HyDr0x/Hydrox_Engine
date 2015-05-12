#version 440 core

#extension GL_ARB_shader_draw_parameters : enable

#define MAXBONES 64

#include "../../../../../include/Shader/Shaderincludes/VertexDeclaration.glslh"
#include "../../../../../include/Shader/Shaderincludes/CameraUBO.glslh"

layout(std430, binding = 0) buffer boneMatrixBuffer
{
	mat4 boneMatrix[];
};

layout(location = POSITION) in vec3 in_Pos;
layout(location = BONEWEIGHTS) in vec4 in_boneWeights;
layout(location = BONEINDICES) in vec4 in_boneIndices;
layout(location = CACHEINDICES0) in vec4 in_cacheIndices0;
layout(location = CACHEINDICES1) in vec4 in_cacheIndices1;

out mat4 vsout_skinningMatrix;
out vec3 vsout_pos3D;
out vec4 vsout_cacheIndices0;
out vec4 vsout_cacheIndices1;
out uint vsout_instanceIndex;

void main()
{
	vsout_skinningMatrix = mat4(0.0f);
	
	vsout_instanceIndex = uint(gl_InstanceID + gl_BaseInstanceARB);
	uint globalInstanceID = MAXBONES * vsout_instanceIndex;
	
	vsout_skinningMatrix += (boneMatrix[globalInstanceID + uint(in_boneIndices.x)] * in_boneWeights.x);
	vsout_skinningMatrix += (boneMatrix[globalInstanceID + uint(in_boneIndices.y)] * in_boneWeights.y);
	vsout_skinningMatrix += (boneMatrix[globalInstanceID + uint(in_boneIndices.z)] * in_boneWeights.z);
	vsout_skinningMatrix += (boneMatrix[globalInstanceID + uint(in_boneIndices.w)] * in_boneWeights.w);

	vsout_cacheIndices0 = in_cacheIndices0;
	vsout_cacheIndices1 = in_cacheIndices1;
	vsout_pos3D = in_Pos;
	gl_Position = viewProjectionMatrix * vsout_skinningMatrix * vec4(in_Pos, 1.0f);
}
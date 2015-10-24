#version 440 core

#extension GL_ARB_shader_draw_parameters : enable

#define MAXBONES 64

#include "../../HydroxShader/include/Shader/Shaderincludes/VertexDeclaration.glslh"
#include "../../HydroxShader/include/Shader/Shaderincludes/CameraUBO.glslh"

layout(std430, binding = 0) buffer boneMatrixBuffer
{
	mat4 boneMatrix[];
};

layout(location = POSITION) in vec3 in_pos;
layout(location = NORMAL) in vec3 in_normal;
layout(location = BONEWEIGHTS) in vec4 in_boneWeights;
layout(location = BONEINDICES) in vec4 in_boneIndices;

out VertexData
{
	vec3 pos3D;
	vec3 normal;
} outData;

void main()
{
	mat4 skinningMatrix = mat4(0.0);
	
	uint instanceIndex = uint(gl_InstanceID + gl_BaseInstanceARB);
	uint globalInstanceID = MAXBONES * instanceIndex;
	
	skinningMatrix += (boneMatrix[globalInstanceID + uint(in_boneIndices.x)] * in_boneWeights.x);
	skinningMatrix += (boneMatrix[globalInstanceID + uint(in_boneIndices.y)] * in_boneWeights.y);
	skinningMatrix += (boneMatrix[globalInstanceID + uint(in_boneIndices.z)] * in_boneWeights.z);
	skinningMatrix += (boneMatrix[globalInstanceID + uint(in_boneIndices.w)] * in_boneWeights.w);

	outData.pos3D = (skinningMatrix * vec4(in_pos, 1.0)).xyz;
	outData.normal = normalize(mat3(skinningMatrix) * in_normal);
	
	gl_Position = viewProjectionMatrix * vec4(outData.pos3D, 1.0);
}
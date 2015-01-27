#version 440 core

#extension ARB_shader_draw_parameters : enable

#define MAXBONES 64

#include "../../../../include/Shader/VertexDeclaration.glslh"
#include "../../../../include/Shader/CameraUBO.glslh"

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

out mat4 vsout_skinningMatrix;
out vec3 vsout_pos3D;
out vec2 vsout_texCoord;
out mat3 vsout_tangentToWorld;
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

	vec3 tangent = cross(in_binormal, in_normal);
	
	mat3 normalWorld = mat3(vsout_skinningMatrix);
	vsout_tangentToWorld[0] = normalWorld * tangent;
	vsout_tangentToWorld[1] = normalWorld * in_binormal;
	vsout_tangentToWorld[2] = normalWorld * in_normal;
	
	//vsout_tangentToWorld = inverse(transpose(vsout_tangentToWorld));

	vsout_pos3D = in_Pos;
	vsout_texCoord = in_texCoord;
	gl_Position = viewProjectionMatrix * vsout_skinningMatrix * vec4(in_Pos, 1.0f);
}
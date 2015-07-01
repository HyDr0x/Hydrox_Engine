#version 440 core

#extension ARB_shader_draw_parameters : enable

#include "../../../../../../include/Shader/Shaderincludes/VertexDeclaration.glslh"
#include "../../../../../../include/Shader/Shaderincludes/LightData.glslh"

#define MAXBONES 64

layout(std430, binding = 0) buffer boneMatrixBuffer
{
	mat4 boneMatrix[];
};

layout(std430, binding = 1) buffer reflectiveShadowLightBuffer
{
	ShadowLightData reflectiveShadowLight[];
};

layout(location = 0) uniform uint lightIndex;

out float vsout_clipDepth;
out vec4 vsout_pos3D;

layout(location = POSITION) in vec4 in_Pos;
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
	
	vsout_pos3D = skinningMatrix * vec4(in_Pos.xyz, 1.0);
	vec3 transformedPosition = (reflectiveShadowLight[lightIndex].lightView * vsout_pos3D).xyz;
	
	transformedPosition.z *= -1.0;
	
	float distance = length(transformedPosition);
	transformedPosition /= distance;//create the normal of the point at the paraboloid to get the texture coordinates
	vsout_clipDepth = transformedPosition.z;
	transformedPosition.z += 1.0;//add the reflection vector(0, 0, 1)
	transformedPosition.xy /= transformedPosition.z;//divide through the new z value to get the normal

	distance = (distance - 0.1) / (1000.0 - 0.1);
	gl_Position = vec4(transformedPosition.x, transformedPosition.y, distance * 2.0 - 1.0, 1);
}
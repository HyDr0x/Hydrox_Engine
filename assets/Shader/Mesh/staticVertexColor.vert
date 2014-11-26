#version 440 core

#extension ARB_shader_draw_parameters : enable

#include "../Header/VertexDeclaration.glslh"
#include "../Header/CameraUBO.glslh"

layout(std430, binding = 0) buffer transformMatrixBuffer
{
	mat4 trfMatrix[];
};

layout(location = POSITION) in vec3 in_Pos;
layout(location = NORMAL) in vec3 in_normal;
layout(location = COLOR) in vec4 in_color;
layout(location = CACHEINDICES0) in vec4 in_cacheIndices0;
layout(location = CACHEINDICES1) in vec4 in_cacheIndices1;

out vec3 vsout_normal;
out vec4 vsout_color;
out vec4 vsout_cacheIndices0;
out vec4 vsout_cacheIndices1;
out uint vsout_instanceIndex;

void main()
{
	vsout_instanceIndex = uint(gl_InstanceID + gl_BaseInstanceARB);
	
	vsout_cacheIndices0 = in_cacheIndices0;
	vsout_cacheIndices1 = in_cacheIndices1;
	vsout_color = in_color;
	vsout_normal = normalize(mat3(trfMatrix[vsout_instanceIndex]) * in_normal);
	
	mat4 MVP = viewProjectionMatrix * trfMatrix[vsout_instanceIndex];
	gl_Position = MVP * vec4(in_Pos, 1);
}
#version 440 core

#extension ARB_shader_draw_parameters : enable

#include "../../../../include/Shader/VertexDeclaration.glslh"
#include "../../../../include/Shader/CameraUBO.glslh"

layout(std430, binding = 0) buffer transformMatrixBuffer
{
	mat4 trfMatrix[];
};

layout(location = POSITION) in vec3 in_Pos;
layout(location = CACHEINDICES0) in vec4 in_cacheIndices0;
layout(location = CACHEINDICES1) in vec4 in_cacheIndices1;

out vec4 vsout_cacheIndices0;
out vec4 vsout_cacheIndices1;
out uint vsout_instanceIndex;

void main()
{
	vsout_cacheIndices0 = in_cacheIndices0;
	vsout_cacheIndices1 = in_cacheIndices1;
	vsout_instanceIndex = uint(gl_InstanceID + gl_BaseInstanceARB);
	
	mat4 MVP = viewProjectionMatrix * trfMatrix[vsout_instanceIndex];
	gl_Position = MVP * vec4(in_Pos, 1);
}
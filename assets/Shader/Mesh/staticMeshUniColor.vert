#version 440 core

#extension ARB_shader_draw_parameters : enable

#include "../Header/CameraUBO.glslh"

layout(std430, binding = 0) buffer transformMatrixBuffer
{
	mat4 trfMatrix[];
};

layout(location = 0) in vec3 in_Pos;
layout(location = 5) in vec3 in_normal;

out vec3 vsout_normal;
flat out uint vsout_instanceIndex;

void main()
{
	vsout_instanceIndex = uint(gl_InstanceID + gl_BaseInstanceARB);

	vsout_normal = mat3(trfMatrix[vsout_instanceIndex]) * in_normal;
	
	mat4 MVP = viewProjectionMatrix * trfMatrix[vsout_instanceIndex];
	gl_Position = MVP * vec4(in_Pos, 1);
}
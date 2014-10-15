#version 440 core

#extension ARB_shader_draw_parameters : enable

#include "../Header/CameraUBO.glslh"

layout(std430, binding = 0) buffer transformMatrixBuffer
{
	mat4 trfMatrix[];
};

layout(location = 0) in vec3 in_Pos;
layout(location = 1) in vec2 in_texCoord;
layout(location = 5) in vec3 in_normal;
layout(location = 6) in vec3 in_binormal;

out vec2 vsout_texCoord;
out mat3 vsout_tangentToWorld;
flat out uint vsout_instanceIndex;

void main()
{
	vsout_instanceIndex = uint(gl_InstanceID + gl_BaseInstanceARB);
	
	mat4 MVP = viewProjectionMatrix * trfMatrix[vsout_instanceIndex];
	
	vec3 tangent = cross(in_normal, in_binormal);
	
	mat3 normalWorld = mat3(trfMatrix[vsout_instanceIndex]);
	vsout_tangentToWorld[0] = normalWorld * in_binormal;
	vsout_tangentToWorld[1] = normalWorld * in_normal;
	vsout_tangentToWorld[2] = normalWorld * tangent;
	
	vsout_texCoord = in_texCoord;
	gl_Position = MVP * vec4(in_Pos, 1);
}
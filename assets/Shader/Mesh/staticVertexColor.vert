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

out vec3 vsout_normal;
out vec4 vsout_color;
out uint vsout_instanceIndex;

void main()
{
	vsout_instanceIndex = uint(gl_InstanceID + gl_BaseInstanceARB);
	
	vsout_color = in_color;
	vsout_normal = normalize(mat3(trfMatrix[vsout_instanceIndex]) * in_normal);
	
	mat4 MVP = viewProjectionMatrix * trfMatrix[vsout_instanceIndex];
	gl_Position = MVP * vec4(in_Pos, 1);
}
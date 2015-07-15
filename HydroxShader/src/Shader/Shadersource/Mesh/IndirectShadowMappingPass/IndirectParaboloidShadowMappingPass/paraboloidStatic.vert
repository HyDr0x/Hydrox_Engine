#version 440 core

#extension ARB_shader_draw_parameters : enable

#include "../../../../../../include/Shader/Shaderincludes/VertexDeclaration.glslh"

layout(std430, binding = 0) buffer transformMatrixBuffer
{
	mat4 trfMatrix[];
};

out vec3 vsout_normal;

layout(location = POSITION) in vec4 in_Pos;
layout(location = NORMAL) in vec3 in_normal;

void main()
{
	vsout_normal = normalize(mat3(trfMatrix[uint(gl_InstanceID + gl_BaseInstanceARB)]) * in_normal);
	
	gl_Position = trfMatrix[uint(gl_InstanceID + gl_BaseInstanceARB)] * vec4(in_Pos.xyz, 1);
}
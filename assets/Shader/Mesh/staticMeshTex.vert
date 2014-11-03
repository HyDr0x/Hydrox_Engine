#version 440 core

#extension ARB_shader_draw_parameters : enable

#include "../Header/VertexDeclaration.glslh"
#include "../Header/CameraUBO.glslh"

layout(std430, binding = 0) buffer transformMatrixBuffer
{
	mat4 trfMatrix[];
};

layout(location = POSITION) in vec3 in_Pos;
layout(location = TEXTURE0) in vec2 in_texCoord;
layout(location = NORMAL) in vec3 in_normal;

out vec2 vsout_texCoord;
out vec3 vsout_normal;
flat out uint vsout_instanceIndex;

void main()
{
	vsout_texCoord = in_texCoord;
	
	vsout_instanceIndex = uint(gl_InstanceID + gl_BaseInstanceARB);

	vsout_normal = mat3(trfMatrix[vsout_instanceIndex]) * in_normal;
	
	mat4 MVP = viewProjectionMatrix * trfMatrix[vsout_instanceIndex];
	gl_Position = MVP * vec4(in_Pos, 1);
}
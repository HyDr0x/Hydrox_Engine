#version 440 core

#extension ARB_shader_draw_parameters : enable

#include "../../../../../include/Shader/Shaderincludes/VertexDeclaration.glslh"
#include "../../../../../include/Shader/Shaderincludes/CameraUBO.glslh"

layout(std430, binding = 0) buffer transformMatrixBuffer
{
	mat4 trfMatrix[];
};

layout(location = POSITION) in vec3 in_Pos;
layout(location = TEXTURE0) in vec2 in_texCoord;
layout(location = NORMAL) in vec3 in_normal;
layout(location = BINORMAL) in vec3 in_binormal;
layout(location = COLOR) in vec4 in_color;

out vec2 vsout_texCoord;
out mat3 vsout_tangentToWorld;
flat out uint vsout_instanceIndex;
out vec4 vsout_color;

void main()
{
	vsout_instanceIndex = uint(gl_InstanceID + gl_BaseInstanceARB);
	
	mat4 MVP = viewProjectionMatrix * trfMatrix[vsout_instanceIndex];

	vec3 tangent = cross(in_normal, in_binormal);
	
	mat3 normalWorld = mat3(trfMatrix[vsout_instanceIndex]);
	vsout_tangentToWorld[0] = normalWorld * tangent;
	vsout_tangentToWorld[1] = normalWorld * in_binormal;
	vsout_tangentToWorld[2] = normalWorld * in_normal;

	vsout_color = in_color;
	vsout_texCoord = in_texCoord;
	gl_Position = MVP * vec4(in_Pos, 1);
}
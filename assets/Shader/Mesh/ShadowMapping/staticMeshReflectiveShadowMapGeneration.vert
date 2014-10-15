#version 440 core

#extension ARB_shader_draw_parameters : enable

#include "../../Header/LightData.glslh"

layout(std430, binding = 0) buffer transformMatrixBuffer
{
	mat4 trfMatrix[];
};

layout(std430, binding = 4) buffer relfectiveShadowLightBuffer
{
	ShadowLightData reflectiveShadowLight[];
};

layout(location = 2) uniform int lightIndex;

layout(location = 0) in vec3 in_Pos;
layout(location = 1) in vec2 in_texCoord;
layout(location = 5) in vec3 in_normal;

out vec4 vsout_pos;
out vec2 vsout_texCoord;
out vec3 vsout_normal;
flat out uint vsout_instanceIndex;

void main()
{
	vsout_instanceIndex = uint(gl_InstanceID + gl_BaseInstanceARB);
	
	vsout_pos = trfMatrix[vsout_instanceIndex] * vec4(in_Pos, 1.0f);
	vsout_texCoord = in_texCoord;
	vsout_normal = mat3(trfMatrix[vsout_instanceIndex]) * in_normal;
	gl_Position = reflectiveShadowLight[lightIndex].lightViewProj * vsout_pos;
}
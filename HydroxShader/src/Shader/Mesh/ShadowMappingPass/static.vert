#version 440 core

#extension ARB_shader_draw_parameters : enable

#include "../../../../include/Shader/VertexDeclaration.glslh"
#include "../../../../include/Shader/LightData.glslh"

layout(std430, binding = 0) buffer transformMatrixBuffer
{
	mat4 trfMatrix[];
};

layout(std430, binding = 4) buffer shadowLightBuffer
{
	ShadowLightData shadowLight[];
};

layout(location = 2) uniform int lightIndex;

layout(location = POSITION) in vec3 in_Pos;

void main()
{																	
	mat4 MVP = shadowLight[lightIndex].lightViewProj * trfMatrix[uint(gl_InstanceID + gl_BaseInstanceARB)];

	gl_Position = MVP * vec4(in_Pos, 1);
}
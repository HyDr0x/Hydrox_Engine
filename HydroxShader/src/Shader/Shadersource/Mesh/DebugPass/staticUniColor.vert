#version 440 core

#extension ARB_shader_draw_parameters : enable

#include "../../../../../include/Shader/Shaderincludes/VertexDeclaration.glslh"
#include "../../../../../include/Shader/Shaderincludes/CameraUBO.glslh"

layout(std430, binding = 0) buffer transformMatrixBuffer
{
	mat4 trfMatrix[];
};

layout(location = POSITION) in vec3 in_Pos;

out vec3 vsout_normal;
flat out uint vsout_instanceIndex;

void main()
{
	gl_Position = (viewProjectionMatrix * (trfMatrix[gl_InstanceID + gl_BaseInstanceARB] * vec4(in_Pos, 1)));
}
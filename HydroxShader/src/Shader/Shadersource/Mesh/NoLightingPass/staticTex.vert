#version 440 core

#extension ARB_shader_draw_parameters : enable

#include "../../HydroxShader/include/Shader/Shaderincludes/VertexDeclaration.glslh"
#include "../../HydroxShader/include/Shader/Shaderincludes/CameraUBO.glslh"

layout(std430, binding = 0) buffer transformMatrixBuffer
{
	mat4 trfMatrix[];
};

layout(location = POSITION) in vec3 in_Pos;
layout(location = TEXTURE0) in vec2 in_texCoord;

out vec2 vsout_texCoord;

void main()
{
	vsout_texCoord = in_texCoord;

gl_Position = (viewProjectionMatrix * (trfMatrix[gl_InstanceID + gl_BaseInstanceARB] * vec4(in_Pos, 1)));
}
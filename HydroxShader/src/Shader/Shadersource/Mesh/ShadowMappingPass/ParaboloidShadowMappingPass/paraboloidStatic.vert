#version 440 core

#extension ARB_shader_draw_parameters : enable

#include "../../HydroxShader/include/Shader/Shaderincludes/VertexDeclaration.glslh"
#include "../../HydroxShader/include/Shader/Shaderincludes/LightData.glslh"

layout(std430, binding = 0) buffer transformMatrixBuffer
{
	mat4 trfMatrix[];
};

layout(std430, binding = 4) buffer shadowLightBuffer
{
	ShadowLightData shadowLight[];
};

layout(location = 2) uniform int lightIndex;

out float vsout_clipDepth;

layout(location = POSITION) in vec4 in_Pos;

void main()
{
	vec3 transformedPosition = (shadowLight[lightIndex].lightView * (trfMatrix[uint(gl_InstanceID + gl_BaseInstanceARB)] * vec4(in_Pos.xyz, 1.0))).xyz;
	
	transformedPosition.z *= -1.0;
	
	float distance = length(transformedPosition);
	transformedPosition /= distance;//create the normal of the point at the paraboloid to get the texture coordinates
	vsout_clipDepth = transformedPosition.z;
	transformedPosition.z += 1.0;//add the reflection vector(0, 0, 1)
	transformedPosition.xy /= transformedPosition.z;//divide through the new z value to get the normal
	
	/*
	float f = 100.0, n = 0.1;
	distance = -distance;
	vsout_depth = (distance * ((-f - n) / (f - n)) - (2.0 * f * n) / (f - n)) / (-distance);*/
	distance = (distance - 0.1) / (100.0 - 0.1);
	gl_Position = vec4(transformedPosition.x, transformedPosition.y, distance * 2.0 - 1.0, 1);
}
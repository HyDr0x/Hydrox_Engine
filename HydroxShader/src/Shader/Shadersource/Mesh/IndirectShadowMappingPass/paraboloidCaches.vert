#version 440 core

#include "../../HydroxShader/include/Shader/Shaderincludes/VertexDeclaration.glslh"

out vec3 vsout_pos3D;
out vec3 vsout_cacheNormal;
out uint vsout_instanceID;
out uint vsout_vertexID;

layout(location = POSITION) in vec4 in_Pos;
layout(location = NORMAL) in vec4 in_cacheNormal;

void main()
{
	vsout_pos3D = in_Pos.xyz;
	vsout_cacheNormal = in_cacheNormal.xyz;
	vsout_instanceID = gl_InstanceID;
	vsout_vertexID = gl_VertexID;
}
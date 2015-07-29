#version 440 core

#include "../../../../../include/Shader/Shaderincludes/VertexDeclaration.glslh"
#include "../../../../../include/Shader/Shaderincludes/Encodings.glslh"

out vec3 vsout_pos3D;
out vec3 vsout_cacheNormal;
out uint vsout_instanceID;
out uint vsout_vertexID;

layout(location = POSITION) in vec4 in_Pos;
layout(location = NORMAL) in vec4 in_cacheNormal;

void main()
{
	vsout_pos3D = in_Pos.xyz;
	vsout_cacheNormal = normalize(decodeNormal(in_cacheNormal.xy));
	vsout_instanceID = gl_InstanceID;
	vsout_vertexID = gl_VertexID;
}
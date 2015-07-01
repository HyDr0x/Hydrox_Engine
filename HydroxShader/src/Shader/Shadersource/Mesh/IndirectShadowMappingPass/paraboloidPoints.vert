#version 440 core

#include "../../../../../include/Shader/Shaderincludes/VertexDeclaration.glslh"

out vec4 vsout_pos3D;
out float vsout_cacheArea;
out uint vsout_instanceID;

layout(location = POSITION) in vec4 in_Pos;
layout(location = SPECIAL0) in float in_cacheArea;

void main()
{
	vsout_pos3D = in_Pos;
	vsout_cacheArea = in_cacheArea;
	vsout_instanceID = gl_InstanceID;
}
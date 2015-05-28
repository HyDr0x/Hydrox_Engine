#version 440 core

#include "../../../../../include/Shader/Shaderincludes/MaterialData.glslh"

layout(location = 0) out vec4 fsout_color;

flat in uint vsout_instanceIndex;

void main()
{
	fsout_color = material[materialIndex[vsout_instanceIndex]].color;
}
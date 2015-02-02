#version 440 core

#define INT32_MAX 2147483647

layout(lines) in;
layout(line_strip, max_vertices = 2) out;

layout(location = 0) uniform uint globalCacheOffset;
layout(location = 1) uniform uint bufferResolution;

flat out uvec4 gsout_cacheIndices[6];

void main()
{
	for(uint i = 0; i < 6; i++)
	{
		gsout_cacheIndices[i] = uvec4(INT32_MAX);
	}

	gl_Position = gl_in[0].gl_Position;
	EmitVertex();

	gl_Position = gl_in[1].gl_Position;
	EmitVertex();
	EndPrimitive();
}
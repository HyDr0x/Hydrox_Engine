#version 440 core

layout(early_fragment_tests) in;

layout(location = 0) out uvec4 fsout_cacheIndices0;
layout(location = 1) out uvec4 fsout_cacheIndices1;
layout(location = 2) out uvec4 fsout_cacheIndices2;
layout(location = 3) out uvec4 fsout_cacheIndices3;
layout(location = 4) out uvec4 fsout_cacheIndices4;
layout(location = 5) out uvec4 fsout_cacheIndices5;

layout(r8, binding = 2) writeonly uniform image2D zBuffer;

layout(location = 1) uniform uint bufferResolution;

flat in uvec4 gsout_cacheIndices[6];

void main()
{
	for(unsigned int i = 0; i < 6; i++)
	{
		imageStore(zBuffer, ivec2(mod(uint(gsout_cacheIndices[i].x), bufferResolution), uint(gsout_cacheIndices[i].x) / bufferResolution), vec4(1));
		imageStore(zBuffer, ivec2(mod(uint(gsout_cacheIndices[i].y), bufferResolution), uint(gsout_cacheIndices[i].y) / bufferResolution), vec4(1));
		imageStore(zBuffer, ivec2(mod(uint(gsout_cacheIndices[i].z), bufferResolution), uint(gsout_cacheIndices[i].z) / bufferResolution), vec4(1));
		imageStore(zBuffer, ivec2(mod(uint(gsout_cacheIndices[i].w), bufferResolution), uint(gsout_cacheIndices[i].w) / bufferResolution), vec4(1));
	}
	
	fsout_cacheIndices0 = gsout_cacheIndices[0];
	fsout_cacheIndices1 = gsout_cacheIndices[1];
	fsout_cacheIndices2 = gsout_cacheIndices[2];
	fsout_cacheIndices3 = gsout_cacheIndices[3];
	fsout_cacheIndices4 = gsout_cacheIndices[4];
	fsout_cacheIndices5 = gsout_cacheIndices[5];
}
#version 440 core

layout(early_fragment_tests) in;

#define INT32_MAX 2147483647

layout(r8, binding = 3) writeonly uniform image2D zBuffer;

layout(location = 1) uniform uint bufferResolution;

flat in uvec4 gsout_cacheIndices[6];

void main()
{
	for(unsigned int i = 0; i < 6; i++)
	{
		if(gsout_cacheIndices[i].x < INT32_MAX) imageStore(zBuffer, ivec2(mod(uint(gsout_cacheIndices[i].x), bufferResolution), uint(gsout_cacheIndices[i].x) / bufferResolution), vec4(1));
		if(gsout_cacheIndices[i].y < INT32_MAX) imageStore(zBuffer, ivec2(mod(uint(gsout_cacheIndices[i].y), bufferResolution), uint(gsout_cacheIndices[i].y) / bufferResolution), vec4(1));
		if(gsout_cacheIndices[i].z < INT32_MAX) imageStore(zBuffer, ivec2(mod(uint(gsout_cacheIndices[i].z), bufferResolution), uint(gsout_cacheIndices[i].z) / bufferResolution), vec4(1));
		if(gsout_cacheIndices[i].w < INT32_MAX) imageStore(zBuffer, ivec2(mod(uint(gsout_cacheIndices[i].w), bufferResolution), uint(gsout_cacheIndices[i].w) / bufferResolution), vec4(1));
	}
}
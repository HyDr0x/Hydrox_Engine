#version 440 core

layout(early_fragment_tests) in;

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
}
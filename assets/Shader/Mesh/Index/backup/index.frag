#version 440 core

layout(early_fragment_tests) in;

layout(location = 0) out vec4 fsout_cacheIndices0;
layout(location = 1) out vec4 fsout_cacheIndices1;
layout(location = 2) out vec4 fsout_cacheIndices2;
layout(location = 3) out vec4 fsout_cacheIndices3;
layout(location = 4) out vec4 fsout_cacheIndices4;
layout(location = 5) out vec4 fsout_cacheIndices5;

layout(std430, binding = 8) buffer zBuffer
{
	uint cacheZBuffer[];
};

flat in vec4 gsout_cacheIndices[6];

void main()
{
	for(unsigned int i = 0; i < 6; i++)
	{
		cacheZBuffer[uint(gsout_cacheIndices[i].x)] = 1;
		cacheZBuffer[uint(gsout_cacheIndices[i].y)] = 1;
		cacheZBuffer[uint(gsout_cacheIndices[i].z)] = 1;
		cacheZBuffer[uint(gsout_cacheIndices[i].w)] = 1;
	}
	
	fsout_cacheIndices0 = gsout_cacheIndices[0] / 299.0f;
	fsout_cacheIndices1 = gsout_cacheIndices[1] / 299.0f;
	fsout_cacheIndices2 = gsout_cacheIndices[2] / 299.0f;
	fsout_cacheIndices3 = gsout_cacheIndices[3] / 299.0f;
	fsout_cacheIndices4 = gsout_cacheIndices[4] / 299.0f;
	fsout_cacheIndices5 = gsout_cacheIndices[5] / 299.0f;
	/*
	fsout_cacheIndices0 = gsout_cacheIndices[0];
	fsout_cacheIndices1 = gsout_cacheIndices[1];
	fsout_cacheIndices2 = gsout_cacheIndices[2];
	fsout_cacheIndices3 = gsout_cacheIndices[3];
	fsout_cacheIndices4 = gsout_cacheIndices[4];
	fsout_cacheIndices5 = gsout_cacheIndices[5];*/
}
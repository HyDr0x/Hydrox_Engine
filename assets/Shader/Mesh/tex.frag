#version 440 core

layout(early_fragment_tests) in;

#include "../Header/MaterialData.glslh"
#include "../Header/CameraUBO.glslh"

layout(location = 0) out vec4 fsout_color;
layout(location = 1) out vec4 fsout_normal;
layout(location = 2) out vec4 fsout_material;

layout(location = 0) uniform sampler2D texSampler;

coherent layout(std430, binding = 3) buffer frameCacheIndexBuffer
{
	uvec4 globalCachesIndices[];
};

layout(std430, binding = 4) buffer zBuffer
{
	uint cacheZBuffer[];
};

in vec2 gsout_texCoord;
in vec3 gsout_normal;
flat in uvec4 gsout_cacheIndices[6];
flat in uint gsout_instanceIndex;

void main()
{
	uint frameBufferIndex = 6 * (uint(gl_FragCoord.x) + uint(gl_FragCoord.y) * width);
	for(uint i = 0; i < 6; i++)
	{
		globalCachesIndices[frameBufferIndex + i] = gsout_cacheIndices[i];
	}
	/*
	for(uint i = 0; i < 6; i++)
	{
		atomicAdd(globalCachesIndices[frameBufferIndex + i].x, 1);
		atomicAdd(globalCachesIndices[frameBufferIndex + i].y, 1);
		atomicAdd(globalCachesIndices[frameBufferIndex + i].z, 1);
		atomicAdd(globalCachesIndices[frameBufferIndex + i].w, 1);
	}*/
	/*
	for(uint i = 0; i < 6; i++)
	{
		globalCachesIndices[frameBufferIndex + i].x = uint(gl_FragCoord.x);
		globalCachesIndices[frameBufferIndex + i].y = uint(gl_FragCoord.y);
		globalCachesIndices[frameBufferIndex + i].z = frameBufferIndex;
		globalCachesIndices[frameBufferIndex + i].w = ~0;
	}*/
	
	for(unsigned int i = 0; i < 6; i++)
	{
		cacheZBuffer[gsout_cacheIndices[i].x] = 1;
		cacheZBuffer[gsout_cacheIndices[i].y] = 1;
		cacheZBuffer[gsout_cacheIndices[i].z] = 1;
		cacheZBuffer[gsout_cacheIndices[i].w] = 1;
	}
	
	fsout_normal = vec4(gsout_normal * 0.5f + 0.5f, 0);
	fsout_color = vec4(texture(texSampler, gsout_texCoord));
	
	MaterialData thisMaterial = material[materialIndex[gsout_instanceIndex]];
	
	fsout_material = vec4(thisMaterial.diffuseStrength, 
												thisMaterial.specularStrength, 
												thisMaterial.ambientStrength, 
												thisMaterial.specularExponent);
}
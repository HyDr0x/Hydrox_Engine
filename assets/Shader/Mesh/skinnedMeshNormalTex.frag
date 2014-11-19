#version 440 core

layout(early_fragment_tests) in;

#include "../Header/MaterialData.glslh"
#include "../Header/CameraUBO.glslh"

layout(location = 0) out vec4 fsout_color;
layout(location = 1) out vec4 fsout_normal;
layout(location = 2) out vec4 fsout_material;

layout(location = 0) uniform sampler2D colorSampler;
layout(location = 1) uniform sampler2D normalSampler;

layout(std430, binding = 3) buffer frameCacheIndexBuffer
{
	uvec4 globalCachesIndices[];
};

layout(std430, binding = 4) buffer zBuffer
{
	uint cacheZBuffer[];
};

in vec2 gsout_texCoord;
in mat3 gsout_tangentToWorld;
flat in uvec4 gsout_cacheIndices[6];
flat in uint gsout_instanceIndex;

void main()
{
/*
	uint frameBufferIndex = 24 * uint(gl_FragCoord.x + gl_FragCoord.y * width);
	for(uint i = 0; i < 24; i += 4)
	{
		uint index = i / 4;
		globalCachesIndices[frameBufferIndex + i + 0] = gsout_cacheIndices[index].x;
		globalCachesIndices[frameBufferIndex + i + 1] = gsout_cacheIndices[index].y;
		globalCachesIndices[frameBufferIndex + i + 2] = gsout_cacheIndices[index].z;
		globalCachesIndices[frameBufferIndex + i + 3] = gsout_cacheIndices[index].w;
	}
	*/
	
	uint frameBufferIndex = 6 * uint(gl_FragCoord.x + gl_FragCoord.y * width);
	for(uint i = 0; i < 6; i++)
	{
		globalCachesIndices[frameBufferIndex + i] = gsout_cacheIndices[i];
	}
	
	for(uint i = 0; i < 6; i++)
	{
		cacheZBuffer[gsout_cacheIndices[i].x] = 1;
		cacheZBuffer[gsout_cacheIndices[i].y] = 1;
		cacheZBuffer[gsout_cacheIndices[i].z] = 1;
		cacheZBuffer[gsout_cacheIndices[i].w] = 1;
	}
	
	vec3 normal = normalize(gsout_tangentToWorld * (texture(normalSampler, gsout_texCoord).xyz * 2.0f - 1.0f));

	fsout_normal = vec4(normal * 0.5f + 0.5f, 0);
	
	fsout_color = texture(colorSampler, gsout_texCoord);
	
	MaterialData thisMaterial = material[materialIndex[gsout_instanceIndex]];
	
	fsout_material = vec4(thisMaterial.diffuseStrength, 
												thisMaterial.specularStrength, 
												thisMaterial.ambientStrength, 
												thisMaterial.specularExponent);
}
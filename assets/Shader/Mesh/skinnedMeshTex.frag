#version 440 core

layout(early_fragment_tests) in;

layout(location = 0) uniform sampler2D texSampler;

#include "../Header/MaterialData.glslh"
#include "../Header/CameraUBO.glslh"

layout(location = 0) out vec4 fsout_color;
layout(location = 1) out vec4 fsout_normal;
layout(location = 2) out vec4 fsout_material;

layout(std430, binding = 3) buffer frameCacheIndexBuffer
{
	uvec4 globalCachesIndices[];
};

layout(std430, binding = 4) buffer zBuffer
{
	uint cacheZBuffer[];
};

in vec3 gsout_normal;
in vec2 gsout_texCoord;
flat in uvec4 gsout_cacheIndices[6];
flat in uint gsout_instanceIndex;

void main()
{
	uint frameBufferIndex = 6 * uint(gl_FragCoord.x + gl_FragCoord.y * width);
	for(uint i = 0; i < 6; i++)
	{
		globalCachesIndices[frameBufferIndex + i] = gsout_cacheIndices[i];
	}
	
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
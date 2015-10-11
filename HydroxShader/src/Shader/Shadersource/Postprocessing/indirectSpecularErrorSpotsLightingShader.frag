#version 440 core

layout(early_fragment_tests) in;

#define PI 3.14159265359
#define INT32_MAX 2147483647
#define UINT16_MAX 65535
#define INT16_MAX 32768

#include "../../../../include/Shader/Shaderincludes/CameraUBO.glslh"
#include "../../../../include/Shader/Shaderincludes/CacheData.glslh"
#include "../../../../include/Shader/Shaderincludes/IndirectLightData.glslh"
#include "../../../../include/Shader/Shaderincludes/Encodings.glslh"

layout(location = 0) uniform sampler2D gBufferDepthSampler;
layout(location = 1) uniform sampler2D gBufferNormalSampler;
layout(location = 2) uniform sampler2D gBufferMaterialSampler;

layout(location = 3) uniform usampler2D cacheIndices[6];

layout(std430, binding = 0) buffer specularCachePositionBuffer
{
	vec4 specularCachePositions[];
};

layout(std430, binding = 1) buffer specularCacheNormalMaterialBuffer
{
	vec4 specularCacheNormalMaterial[];
};

layout(std430, binding = 2) buffer proxyLightPositionBuffer
{
	vec4 proxyLightPosition[];
};

layout(std430, binding = 3) buffer proxyLightLuminousFluxBuffer
{
	vec4 proxyLightLuminousFlux[];
};

out vec4 luminousFlux;

in vec2 gsout_texCoord;

void main()
{
	ivec2 texCoord = ivec2(gl_FragCoord.x, gl_FragCoord.y);
	
	uvec4 tmpIndices;
	uint cacheIndex[24];
	
	for(uint i = 0; i < 6; i++)
	{
		tmpIndices = texelFetch(cacheIndices[i], texCoord, 0);
		for(uint j = 0; j < 4; j++)
		{
			cacheIndex[i * 4 + j] = tmpIndices[j];
		}
	}

	vec4 pos3D = vec4(gsout_texCoord, texture(gBufferDepthSampler, gsout_texCoord).r, 1.0);
	pos3D.xyz = pos3D.xyz * 2.0 - 1.0;
	pos3D = invViewProjectionMatrix * pos3D;
	pos3D.xyz /= pos3D.w;

	uint validIndexCounter = 0;
	float dmax = 0.0;
	for(uint i = 0; i < 24; i++)
	{
		if(cacheIndex[i] < UINT16_MAX - 1)
		{
			validIndexCounter++;
			vec3 diff = pos3D.xyz - (specularCachePositions[cacheIndex[i]].xyz);
			dmax = max(dmax, dot(diff, diff));
		}
	}
	
	if(validIndexCounter == 0) discard;
	
	vec3 normal = normalize(texture(gBufferNormalSampler, gsout_texCoord).xyz * 2.0 - 1.0);
	vec4 material = texture(gBufferMaterialSampler, gsout_texCoord);
	
	vec3 camDir = normalize(eyePos.xyz - pos3D.xyz);
	vec3 reflectCamDir = reflect(-camDir, normal);
	
	dmax = sqrt(max(dmax, 0.00001));

	vec3 Xpg = vec3(0.0), phiPG = vec3(0.0);
	float wGesG = 0.0, cacheProxyMinDistanceG = 0.0;
	
	for(uint i = 0; i < 24; i++)
	{
		if(cacheIndex[i] < UINT16_MAX)
		{
			CacheData cache;
			cache.position = specularCachePositions[cacheIndex[i]];
			cache.normal = specularCacheNormalMaterial[cacheIndex[i]];
			vec3 cacheNormal = normalize(cache.normal.xyz);
			
			float dir = max(1.0 - length(pos3D.xyz - cache.position.xyz) / dmax, 0.0);
				
			vec3 camCacheDir = normalize(eyePos.xyz - cache.position.xyz);
			float wg = dir * sqrt(max(dot(reflect(-camCacheDir, cacheNormal), reflectCamDir), 0.0));
			
			IndirectLightData indirectLightG;
			indirectLightG.position = proxyLightPosition[cacheIndex[i]];
			indirectLightG.luminousFlux = proxyLightLuminousFlux[cacheIndex[i]];
			
			cacheProxyMinDistanceG += wg * indirectLightG.position.w;
			Xpg += wg * indirectLightG.position.xyz;
			phiPG += wg * indirectLightG.luminousFlux.xyz;
			wGesG += wg;
		}
	}
	
	Xpg = wGesG > 0 ? Xpg / wGesG : vec3(0);
	phiPG = wGesG > 0 ? phiPG / wGesG : vec3(0);
	cacheProxyMinDistanceG = wGesG > 0 ? cacheProxyMinDistanceG / wGesG : 0;
	
	vec3 lightDirG = Xpg - pos3D.xyz;
	//float lengthG = max(dot(lightDirG, lightDirG), dot(cacheProxyMinDistanceG, cacheProxyMinDistanceG));
	float lengthG = max(length(lightDirG), cacheProxyMinDistanceG);
	lightDirG = normalize(lightDirG);
	float frg = material.y * max(dot(lightDirG, normal), 0.0) * pow(max(dot(reflect(-lightDirG, normal), camDir), 0.0), material.w);

	//luminousFlux = vec4(normal, 1);
	//luminousFlux = vec4(vec3(frg), 1);
	//luminousFlux = vec4(0.05 * vec3(lengthG), 1);
	//luminousFlux = vec4(Xpg, 1);
	//luminousFlux = vec4(0.001 * phiPG, 1);
	luminousFlux = vec4((frg * phiPG) / (4.0 * PI * lengthG), 1.0);
	//luminousFlux = vec4(1, 0, 0, 0);
}
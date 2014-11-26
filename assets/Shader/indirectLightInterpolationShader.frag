#version 440 core

#define INT32_MAX 2147483647
#define PI 3.14159265359f

#include "Header/CameraUBO.glslh"
#include "Header/CacheData.glslh"
#include "Header/IndirectLightData.glslh"

layout(location = 0) uniform sampler2D depthSampler;
layout(location = 1) uniform sampler2D normalSampler;
layout(location = 2) uniform sampler2D materialSampler;

layout(std430, binding = 0) buffer globalCacheBuffer
{
	CacheData caches[];
};

layout(std430, binding = 1) buffer frameCacheIndicesBuffer
{
	uint frameCacheIndices[];
};

layout(std430, binding = 2) buffer indirectLightDataBuffer
{
	IndirectLightData indirectLights[];
};

out vec4 luminousFlux;

in vec2 gsout_texCoord;

void main()
{
	vec4 tmpPos = vec4(gsout_texCoord, texture(depthSampler, gsout_texCoord).r, 1.0f);
	
	if(tmpPos.z == 1.0f) //discard the lighting e.g. for skybox or billboards
	{
		luminousFlux = vec4(1.0f);
		return;
	}
	
	tmpPos.xyz = tmpPos.xyz * 2.0f - 1.0f;
	tmpPos = invViewProjectionMatrix * tmpPos;
	tmpPos /= tmpPos.w;
	vec3 pos = tmpPos.xyz;
	
	vec4 material = texture(materialSampler, gsout_texCoord);
	vec3 normal = normalize(texture(normalSampler, gsout_texCoord).xyz * 2.0f - 1.0f);//vec3(0,1,0);
	
	vec3 camDir = normalize(eyePos.xyz - pos);
	vec3 reflectCamDir = reflect(-camDir, normal);
	
	uint index = (uint(gl_FragCoord.x) + uint(gl_FragCoord.y) * width) * 24;
	
	float dmax = 0.0f;
	for(uint i = 0; i < 24; i++)
	{
		uint cacheIndex = frameCacheIndices[index + i];
		if(cacheIndex < INT32_MAX)
		{
			CacheData cache = caches[cacheIndex];
			vec3 diff = pos - cache.position.xyz;
			dmax = max(dmax, dot(diff, diff));
		}
	}
	
	dmax = sqrt(dmax);
	
	vec3 Xpd = vec3(0.0f), phiPD = vec3(0.0f);
	vec3 Xpg = vec3(0.0f), phiPG = vec3(0.0f);
	
	float wGesD = 0.0f;
	float wGesG = 0.0f;
	
	for(uint i = 0; i < 24; i++)
	{
		uint cacheIndex = frameCacheIndices[index + i];
		if(cacheIndex < INT32_MAX)
		{
			CacheData cache = caches[cacheIndex];
			
			float dir = 1.0f - length(pos - cache.position.xyz) / dmax;
			
			float wd = dir * sqrt(max(dot(cache.normal.xyz, normal), 0));
			
			vec3 camCacheDir = normalize(eyePos.xyz - cache.position.xyz);
			vec3 reflectCamCacheDir = reflect(-camCacheDir, cache.normal.xyz);
			
			float wg = dir * sqrt(max(dot(reflectCamCacheDir, reflectCamDir), 0));
			
			IndirectLightData indirectLightD = indirectLights[2 * cacheIndex + 0];
			IndirectLightData indirectLightG = indirectLights[2 * cacheIndex + 1];
			
			Xpd += wd * indirectLightD.position.xyz;
			Xpg += wg * indirectLightG.position.xyz;
			
			phiPD +=wd * indirectLightD.luminousFlux.xyz;
			phiPG += wg * indirectLightG.luminousFlux.xyz;
			
			wGesD += wd;
			wGesG += wg;
		}
	}
	
	Xpd /= wGesD;
	Xpg /= wGesG;
	
	phiPD /= wGesD;
	phiPG /= wGesG;
	
	vec3 lightDirD = normalize(Xpd - pos);
	float frd = material.x * max(dot(lightDirD, normal), 0);
	
	vec3 lightDirG = normalize(Xpg - pos);
	vec3 reflectLightDirG = reflect(-lightDirG, normal);
	float frg = material.y * pow(max(dot(reflectLightDirG, camDir), 0), material.w);
	
	luminousFlux = vec4(float(frameCacheIndices[index]) / 299.0f, 0, 0, 1.0f);//vec4((frd * phiPD) / (4.0f * PI * length(pos - Xpd)), 1.0f);
	//luminousFlux = vec4((frd * phiPD) / (4.0f * PI * length(pos - Xpd)) + (frg * phiPG) / (4.0f * PI * length(pos - Xpg)), 1.0f);
}
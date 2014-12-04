#version 440 core

#define INT32_MAX 2147483647
#define PI 3.14159265359f

#include "Header/CameraUBO.glslh"
#include "Header/CacheData.glslh"
#include "Header/IndirectLightData.glslh"

layout(location = 0) uniform sampler2D depthSampler;
layout(location = 1) uniform sampler2D normalSampler;
layout(location = 2) uniform sampler2D materialSampler;

layout(location = 3) uniform usampler2DArray cacheIndexSampler;

layout(std430, binding = 0) buffer globalCacheBuffer
{
	CacheData caches[];
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
	
	if(tmpPos.z == 1.0f) //discard the indirect lighting e.g. for skybox or billboards
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
	
	uint cacheIndices[24];
	for(uint i = 0; i < 6; i++)
	{
		cacheIndices[(4 * i) + 0] = texture(cacheIndexSampler, vec3(gsout_texCoord, i)).x;
		cacheIndices[(4 * i) + 1] = texture(cacheIndexSampler, vec3(gsout_texCoord, i)).y;
		cacheIndices[(4 * i) + 2] = texture(cacheIndexSampler, vec3(gsout_texCoord, i)).z;
		cacheIndices[(4 * i) + 3] = texture(cacheIndexSampler, vec3(gsout_texCoord, i)).w;
	}
	
	float dmax = 0.0f;
	for(uint i = 0; i < 24; i++)
	{
		if(cacheIndices[i] < INT32_MAX)
		{
			vec3 diff = pos - caches[cacheIndices[i]].position.xyz;
			dmax = max(dmax, dot(diff, diff));
		}
	}
	
	dmax = sqrt(dmax);
	
	vec3 Xpd = vec3(0.0f), phiPD = vec3(0.0f);
	vec3 Xpg = vec3(0.0f), phiPG = vec3(0.0f);
	
	float wGesD = 0.0f;
	float wGesG = 0.0f;
	uint counter = 0;
	for(uint i = 0; i < 24; i++)
	{
		if(cacheIndices[i] < INT32_MAX)
		{
			CacheData cache = caches[cacheIndices[i]];
			
			float dir = max(1.0f - length(pos - cache.position.xyz) / dmax, 0);
			
			float wd = dir * sqrt(max(dot(cache.normal.xyz, normal), 0));
			
			vec3 camCacheDir = normalize(eyePos.xyz - cache.position.xyz);
			vec3 reflectCamCacheDir = reflect(-camCacheDir, cache.normal.xyz);
			
			float wg = dir * sqrt(max(dot(reflectCamCacheDir, reflectCamDir), 0));
			
			IndirectLightData indirectLightD = indirectLights[2 * cacheIndices[i] + 0];
			IndirectLightData indirectLightG = indirectLights[2 * cacheIndices[i] + 1];
			
			Xpd += wd * indirectLightD.position.xyz;
			Xpg += wg * indirectLightG.position.xyz;
			/*
			vec3 singlePhiPD = clamp(wd * indirectLightD.luminousFlux.xyz, vec3(0), vec3(65500));
			if(65500 - phiPD.x > singlePhiPD.x && 65500 - phiPD.y > singlePhiPD.y && 65500 - phiPD.z > singlePhiPD.z)
				phiPD += singlePhiPD;
				
			vec3 singlePhiPG = clamp(wg * indirectLightG.luminousFlux.xyz, vec3(0), vec3(65500));
			if(65500 - phiPG.x > singlePhiPG.x && 65500 - phiPG.y > singlePhiPG.y && 65500 - phiPG.z > singlePhiPG.z)
				phiPG += singlePhiPG;*/
				
			phiPD += wd * indirectLightD.luminousFlux.xyz;
			phiPG += wg * indirectLightG.luminousFlux.xyz;
			
			wGesD += wd;
			wGesG += wg;
		}
	}

	Xpd /= wGesD;
	Xpg /= wGesG;
	
	phiPD /= wGesD;
	phiPG /= wGesG;
		
	vec3 lightDirD = Xpd - pos;
	float lengthD = length(lightDirD);
	lightDirD = normalize(lightDirD);
	float frd = material.x * max(dot(lightDirD, normal), 0);
			
	vec3 lightDirG = Xpg - pos;
	float lengthG = length(lightDirG);
	lightDirG = normalize(lightDirG);
	vec3 reflectLightDirG = reflect(-lightDirG, normal);
	float frg = material.y * pow(max(dot(reflectLightDirG, camDir), 0), material.w);
	
	//luminousFlux = vec4(phiPD / 1000.0f, 1);
	//luminousFlux = vec4(phiPD, 1);
	//luminousFlux = vec4((frd * phiPD) / (4.0f * PI * lengthD), 1.0f);
	//luminousFlux = vec4((frg * phiPG) / (4.0f * PI * lengthG), 1.0f);
	luminousFlux = vec4((frd * phiPD) / (4.0f * PI * lengthD) + (frg * phiPG) / (4.0f * PI * lengthG), 1.0f);
}
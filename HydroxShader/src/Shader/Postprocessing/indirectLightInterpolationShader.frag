#version 440 core

#define INT32_MAX 2147483647
#define PI 3.14159265359f

#include "../../../include/Shader/CameraUBO.glslh"
#include "../../../include/Shader/CacheData.glslh"
#include "../../../include/Shader/IndirectLightData.glslh"
#include "../../../include/Shader/Encodings.glslh"

layout(location = 0) uniform sampler2D depthSampler;
layout(location = 1) uniform sampler2D normalSampler;
layout(location = 2) uniform sampler2D materialSampler;

layout(location = 3) uniform usampler2DArray cacheIndexSampler;

layout(rgba32f, binding = 0) readonly uniform image2D globalCachePositionBuffer;
layout(rgba32f, binding = 1) readonly uniform image2D globalCacheNormalBuffer;

layout(rgba32f, binding = 2) readonly uniform image2D indirectLightPositionDBuffer;
layout(rgba32f, binding = 3) readonly uniform image2D indirectLightLuminousFluxDBuffer;

layout(rgba32f, binding = 4) readonly uniform image2D indirectLightPositionGBuffer;
layout(rgba32f, binding = 5) readonly uniform image2D indirectLightLuminousFluxGBuffer;

layout(location = 8) uniform uint bufferResolution;

out vec4 luminousFlux;

in vec2 gsout_texCoord;

void main()
{
	vec4 tmpPos = vec4(gsout_texCoord, texture(depthSampler, gsout_texCoord).r, 1.0f);
	
	if(tmpPos.z == 1.0f) //discard the indirect lighting for the background e.g. for skybox
	{
		luminousFlux = vec4(0.0f);
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
		uvec4 indices = texture(cacheIndexSampler, vec3(gsout_texCoord, i));
		cacheIndices[(4 * i) + 0] = indices.x;
		cacheIndices[(4 * i) + 1] = indices.y;
		cacheIndices[(4 * i) + 2] = indices.z;
		cacheIndices[(4 * i) + 3] = indices.w;
	}
	
	float dmax = 0.0f;
	for(uint i = 0; i < 24; i++)
	{
		if(cacheIndices[i] < INT32_MAX)
		{
			ivec2 coord = ivec2(mod(cacheIndices[i], bufferResolution), cacheIndices[i] / bufferResolution);
			vec3 diff = pos - imageLoad(globalCachePositionBuffer, coord).xyz;
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
		if(cacheIndices[i] < INT32_MAX)
		{
			ivec2 coord = ivec2(mod(cacheIndices[i], bufferResolution), cacheIndices[i] / bufferResolution);
			CacheData cache;
			cache.position = imageLoad(globalCachePositionBuffer, coord);
			cache.normal = imageLoad(globalCacheNormalBuffer, coord);
			
			vec3 cacheNormal = normalize(decodeNormal(cache.normal.xy));
			
			float dir = max(1.0f - length(pos - cache.position.xyz) / dmax, 0.0001f);
			
			float wd = dir * sqrt(max(dot(cacheNormal, normal), 0));
			
			vec3 camCacheDir = normalize(eyePos.xyz - cache.position.xyz);
			float wg = dir * sqrt(max(dot(reflect(-camCacheDir, cacheNormal), reflectCamDir), 0));
			
			IndirectLightData indirectLightD;
			indirectLightD.position = imageLoad(indirectLightPositionDBuffer, coord);
			indirectLightD.luminousFlux = imageLoad(indirectLightLuminousFluxDBuffer, coord);
			
			IndirectLightData indirectLightG;
			indirectLightG.position = imageLoad(indirectLightPositionGBuffer, coord);
			indirectLightG.luminousFlux = imageLoad(indirectLightLuminousFluxGBuffer, coord);
			
			Xpd += wd * indirectLightD.position.xyz;
			Xpg += wg * indirectLightG.position.xyz;
				
			phiPD += wd * indirectLightD.luminousFlux.xyz;
			phiPG += wg * indirectLightG.luminousFlux.xyz;
			
			wGesD += wd;
			wGesG += wg;
		}
	}

	Xpd = wGesD > 0 ? Xpd / wGesD : vec3(0);
	Xpg = wGesG > 0 ? Xpg / wGesG : vec3(0);
	
	phiPD = wGesD > 0 ? phiPD / wGesD : vec3(0);
	phiPG = wGesG > 0 ? phiPG / wGesG : vec3(0);
		
	vec3 lightDirD = Xpd - pos;
	float lengthD = dot(lightDirD, lightDirD);//phd says: length(lightDirD);
	lightDirD = normalize(lightDirD);
	float frd = material.x * max(dot(lightDirD, normal), 0);
			
	vec3 lightDirG = Xpg - pos;
	float lengthG = dot(lightDirG, lightDirG);//phd says: length(lightDirG);
	lightDirG = normalize(lightDirG);
	vec3 reflectLightDirG = reflect(-lightDirG, normal);
	float frg = max(dot(lightDirG, normal), 0) * material.y * pow(max(dot(reflectLightDirG, camDir), 0), material.w);
	
	//luminousFlux = vec4(1, 0, 0, 1);
	luminousFlux = vec4(phiPD / 2000.0f, 1);
	//luminousFlux = vec4(wGesD / 10.0f, 0, 0, 0);
	//luminousFlux = vec4((frd * phiPD) / (4.0f * PI * lengthD), 1.0f);
	//luminousFlux = vec4((frg * phiPG) / (4.0f * PI * lengthG), 1.0f);
	//luminousFlux = vec4((frd * phiPD) / (4.0f * PI * lengthD) + max((frg * phiPG) / (4.0f * PI * lengthG), 0.0f), 1.0f);
}
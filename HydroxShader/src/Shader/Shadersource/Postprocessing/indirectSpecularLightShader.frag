#version 440 core

layout(early_fragment_tests) in;

#define PI 3.14159265359
#define INT32_MAX 2147483647
#define INT16_MAX 32768

#include "../../HydroxShader/include/Shader/Shaderincludes/CameraUBO.glslh"
#include "../../HydroxShader/include/Shader/Shaderincludes/CacheData.glslh"
#include "../../HydroxShader/include/Shader/Shaderincludes/IndirectLightData.glslh"
#include "../../HydroxShader/include/Shader/Shaderincludes/Encodings.glslh"

layout(location = 0) uniform sampler2D gBufferDepthSampler;
layout(location = 1) uniform sampler2D gBufferNormalSampler;
layout(location = 2) uniform sampler2D gBufferMaterialSampler;

layout(rgba16ui, binding = 0) readonly uniform uimage2D cacheIndexMap0;
layout(rgba16ui, binding = 1) readonly uniform uimage2D cacheIndexMap1;

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
	
	uvec4 tmpIndices = imageLoad(cacheIndexMap0, texCoord);
	uint cacheIndex[8];
	cacheIndex[0] = tmpIndices.r;
	cacheIndex[1] = tmpIndices.g;
	cacheIndex[2] = tmpIndices.b;
	cacheIndex[3] = tmpIndices.a;

	tmpIndices = imageLoad(cacheIndexMap1, texCoord);
	cacheIndex[4] = tmpIndices.r;
	cacheIndex[5] = tmpIndices.g;
	cacheIndex[6] = tmpIndices.b;
	cacheIndex[7] = tmpIndices.a;
	
	vec4 pos3D = vec4(gsout_texCoord, texture(gBufferDepthSampler, gsout_texCoord).r, 1.0);
	pos3D.xyz = pos3D.xyz * 2.0 - 1.0;
	pos3D = invViewProjectionMatrix * pos3D;
	pos3D.xyz /= pos3D.w;

	vec3 normal = normalize(texture(gBufferNormalSampler, gsout_texCoord).xyz * 2.0 - 1.0);
	vec4 material = texture(gBufferMaterialSampler, gsout_texCoord);
	
	vec3 camDir = normalize(eyePos.xyz - pos3D.xyz);
	vec3 reflectCamDir = reflect(-camDir, normal);
	
	float dmax = 0.0;
	for(uint i = 0; i < 8; i++)
	{
		if(cacheIndex[i] < INT16_MAX)
		{
			vec3 diff = pos3D.xyz - (specularCachePositions[cacheIndex[i]].xyz);
			dmax = max(dmax, dot(diff, diff) + 0.00001);
		}
	}
	
	dmax = sqrt(dmax);

	vec3 Xpg = vec3(0.0f), phiPG = vec3(0.0f);
	float wGesG = 0.0f, cacheProxyMinDistanceG = 0.0f;
	
	for(uint i = 0; i < 8; i++)
	{
		if(cacheIndex[i] < INT16_MAX)
		{
			CacheData cache;
			cache.position = specularCachePositions[cacheIndex[i]];
			
			vec4 cacheNormalMaterial = specularCacheNormalMaterial[cacheIndex[i]];
			cache.normal.xyz = normalize(decodeNormal(cacheNormalMaterial.xy));
			
			float dist = max(length(pos3D.xyz - cache.position.xyz), 0.0001);

			float wg = dmax / dist;
			
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
	luminousFlux = vec4(max((frg * phiPG) / (4.0 * PI * lengthG), vec3(0)), 1.0);
	//luminousFlux = texture(indirectShadowMapSampler, texCoord).r * vec4(max((frg * phiPG) / (4 * PI * lengthG), vec3(0)), 1.0);
}
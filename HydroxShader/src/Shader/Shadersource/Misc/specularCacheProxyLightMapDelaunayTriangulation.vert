#version 440 core

#define INT32_MAX 2147483647

#include "../../HydroxShader/include/Shader/Shaderincludes/VertexDeclaration.glslh"
#include "../../HydroxShader/include/Shader/Shaderincludes/CameraUBO.glslh"
#include "../../HydroxShader/include/Shader/Shaderincludes/CacheData.glslh"
#include "../../HydroxShader/include/Shader/Shaderincludes/Encodings.glslh"
#include "../../HydroxShader/include/Shader/Shaderincludes/IndirectLightData.glslh"

layout(std430, binding = 0) buffer specularCachePositionBuffer
{
	vec4 specularCachePositions[];
};

layout(std430, binding = 1) buffer specularCacheNormalMaterialBuffer
{
	vec4 specularCacheNormals[];
};

layout(std430, binding = 2) buffer proxyLightPositionBuffer
{
	vec4 proxyLightPosition[];
};

layout(std430, binding = 3) buffer proxyLightLuminousFluxBuffer
{
	vec4 proxyLightLuminousFlux[];
};

layout(location = POSITION) in vec4 in_pos;
layout(location = NORMAL) in vec4 in_normal;
layout(location = CACHEINDICES0) in vec4 in_cacheIndices0;
layout(location = CACHEINDICES1) in vec4 in_cacheIndices1;

out float vsout_depth;
out vec3 vsout_normal;
out vec3 vsout_Xpd;
out vec3 vsout_phiPD;
out vec3 vsout_Xpg;
out vec3 vsout_phiPG;
out float vsout_cacheProxyMinDistanceD;
out float vsout_cacheProxyMinDistanceG;
out vec4 vsout_cacheIndices0;
out vec4 vsout_cacheIndices1;

void main()
{
	vsout_cacheIndices0 = in_cacheIndices0;
	vsout_cacheIndices1 = in_cacheIndices1;

	vec4 pos3D = viewProjectionMatrix * vec4(in_pos.xyz, 1.0);

	vsout_depth = 2.0 * near * far / (far + near - (0.5 * (pos3D.z / pos3D.w) + 0.5) * (far - near));
	
	ivec2 texCoord = ivec2(((pos3D.xy / pos3D.w) * 0.5 + 0.5) * vec2(width, height));
	
	vsout_normal = normalize(in_normal.xyz);
	
	vec3 camDir = normalize(eyePos.xyz - in_pos.xyz);
	vec3 reflectCamDir = reflect(-camDir, vsout_normal);

	uint cacheIndices[8];
	cacheIndices[0] = uint(in_cacheIndices0.x);
	cacheIndices[1] = uint(in_cacheIndices0.y);
	cacheIndices[2] = uint(in_cacheIndices0.z);
	cacheIndices[3] = uint(in_cacheIndices0.w);
	cacheIndices[4] = uint(in_cacheIndices1.x);
	cacheIndices[5] = uint(in_cacheIndices1.y);
	cacheIndices[6] = uint(in_cacheIndices1.z);
	cacheIndices[7] = uint(in_cacheIndices1.w);

	float dmax = 0.0;
	for(uint i = 0; i < 8; i++)
	{
		if(cacheIndices[i] < INT32_MAX)
		{
			vec3 diff = in_pos.xyz - specularCachePositions[cacheIndices[i]].xyz;
			dmax = max(dmax, dot(diff, diff));
		}
	}
	
	dmax = sqrt(max(dmax, 0.0001));
	
	vec3 Xpd = vec3(0.0), phiPD = vec3(0.0);
	vec3 Xpg = vec3(0.0), phiPG = vec3(0.0);
	
	float wGesD = 0.0;
	float wGesG = 0.0;
	
	vsout_cacheProxyMinDistanceD = 0.0;
	vsout_cacheProxyMinDistanceG = 0.0;
		
	for(uint i = 0; i < 8; i++)
	{
		if(cacheIndices[i] < INT32_MAX)
		{
			CacheData cache;
			cache.position = specularCachePositions[cacheIndices[i]];
			cache.normal = specularCacheNormals[cacheIndices[i]];
			
			vec3 cacheNormal = normalize(decodeNormal(cache.normal.xy));
			
			float dir = max(1.0 - length(in_pos.xyz - cache.position.xyz) / dmax, 0.0);
			
			float wd = dir * sqrt(max(dot(vsout_normal, cacheNormal), 0.0));
			
			vec3 camCacheDir = normalize(eyePos.xyz - cache.position.xyz);
			float wg = dir * sqrt(max(dot(reflect(-camCacheDir, cacheNormal), reflectCamDir), 0.0));
			
			IndirectLightData indirectLightD;
			indirectLightD.position = proxyLightPosition[2 * cacheIndices[i]];
			indirectLightD.luminousFlux = proxyLightLuminousFlux[2 * cacheIndices[i]];
			
			IndirectLightData indirectLightG;
			indirectLightG.position = proxyLightPosition[2 * cacheIndices[i] + 1];
			indirectLightG.luminousFlux = proxyLightLuminousFlux[2 * cacheIndices[i] + 1];
			
			vsout_cacheProxyMinDistanceD += wd * indirectLightD.position.w;
			vsout_cacheProxyMinDistanceG += wg * indirectLightG.position.w;
			
			Xpd += wd * indirectLightD.position.xyz;
			Xpg += wg * indirectLightG.position.xyz;
				
			phiPD += wd * indirectLightD.luminousFlux.xyz;
			phiPG += wg * indirectLightG.luminousFlux.xyz;
			
			wGesD += wd;
			wGesG += wg;
		}
	}

	vsout_Xpd = wGesD > 0 ? Xpd / wGesD : vec3(0);
	vsout_Xpg = wGesG > 0 ? Xpg / wGesG : vec3(0);
	
	vsout_phiPD = wGesD > 0 ? phiPD / wGesD : vec3(0);
	vsout_phiPG = wGesG > 0 ? phiPG / wGesG : vec3(0);
	
	vsout_cacheProxyMinDistanceD = wGesD > 0 ? vsout_cacheProxyMinDistanceD / wGesD : 0;
	vsout_cacheProxyMinDistanceG = wGesG > 0 ? vsout_cacheProxyMinDistanceG / wGesG : 0;

	gl_Position = pos3D;
}
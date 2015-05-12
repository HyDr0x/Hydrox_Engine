#version 440 core

#define AREAARRAYSIZE 25
#define SAMPLERNUMBER
#define PI 3.14159265359
#define EPSILON 0.00001
#define INT32_MAX 2147483647

#include "../../../../include/Shader/Shaderincludes/LightData.glslh"
#include "../../../../include/Shader/Shaderincludes/IndirectLightData.glslh"
#include "../../../../include/Shader/Shaderincludes/CacheData.glslh"
#include "../../../../include/Shader/Shaderincludes/CameraUBO.glslh"
#include "../../../../include/Shader/Shaderincludes/Encodings.glslh"

layout(binding = 1) uniform samplingPattern
{
	vec4 samples[SAMPLERNUMBER];
};

layout(std430, binding = 2) buffer reflectiveShadowLightBuffer
{
	ShadowLightData reflectiveShadowLight[];
};

layout(location = 0) out vec4 indirectLightPositionD;
layout(location = 1) out vec4 indirectLightLuminousFluxD;
layout(location = 2) out vec4 indirectLightPositionG;
layout(location = 3) out vec4 indirectLightLuminousFluxG;

layout(location = 0) uniform sampler2DArray indirectLightPosSampler;
layout(location = 1) uniform sampler2DArray indirectLightNormalSampler;
layout(location = 2) uniform sampler2DArray indirectLightLuminousFluxSampler;

layout(location = 3) uniform sampler2D globalCachePositionBuffer;
layout(location = 4) uniform sampler2D globalCacheNormalBuffer;
layout(location = 5) uniform sampler2D globalCacheAreaBuffer;

layout(location = 6) uniform sampler2D zBuffer;

layout(location = 7) uniform uint reflectiveShadowMapWidth;
layout(location = 8) uniform uint reflectiveShadowMapNumber;

layout(location = 9) uniform uint cacheBufferResolution;

layout(location = 10) uniform uint cacheNumber;

in vec2 gsout_texCoord;

vec3 project(in vec3 t, in vec3 b, in vec3 c, in vec3 p)
{
	vec3 diff = normalize(p - c);//normalization to project the point onto the hemisphere
	
	float shareT = dot(diff, t);
	float shareB = dot(diff, b);
	
	return c + t * shareT + b * shareB;
}

float calculateProjectedProxyLightArea(in vec3 cachePos, in vec3 cacheNormal, in vec3 lightPos, in float r)
{
	vec3 lightDir = lightPos - cachePos;
	float cosAngle = max(dot(cacheNormal, normalize(lightDir)), 0.0);
	
	return (r * r * cosAngle) / (r * r + dot(lightDir, lightDir));
}

float calculateProjectedCacheArea(in vec3 cachePos0, in vec3 cacheNormal0, in vec3 cachePos1, in vec3 cacheNormal1, in float cacheArea)
{
	vec3 cacheDir = cachePos1 - cachePos0;
	
	float cosAngle0 = max(dot(cacheNormal0, normalize(cacheDir)), 0.0);
	float cosAngle1 = abs(dot(cacheNormal1, normalize(cacheDir)));
	
	return max((cacheArea * cosAngle0 * cosAngle1) / (cacheArea + PI * dot(cacheDir, cacheDir)), 0.0);
}

float overlappingArea(in vec3 p0, in float length0, in vec3 p1, in float length1, in vec3 t, in vec3 b, inout vec2 areaHeuristicMin, inout vec2 areaHeuristicMax)
{
	vec3 diff = p1 - p0;
	
	float shareT = abs(dot(diff, t));
	float shareB = abs(dot(diff, b));
	
	float minLength = min(length0, length1) * 2.0;//use the full length of the square, not only the half
	float ax = clamp((length0 + length1) - abs(shareT), 0.0, minLength);
	float ay = clamp((length0 + length1) - abs(shareB), 0.0, minLength);
	
	areaHeuristicMin = min(areaHeuristicMin, vec2(shareT - length1, shareB - length1));
	areaHeuristicMax = max(areaHeuristicMax, vec2(shareT + length1, shareB + length1));
	
	return ax * ay;
}

void calculateVisibility(in CacheData cache, in vec3 Xpd, in vec3 Xpg, in vec3 Xpdq, in vec3 Xpgq, out float visibilityD, out float visibilityG)
{
	float areaD = 0.0;
	float areaG = 0.0;
	
	vec3 binormal = normalize(cross(vec3(0.267261241, 0.534522483, 0.801783725), cache.normal.xyz));
	vec3 tangent = normalize(cross(binormal, cache.normal.xyz));
	
	vec3 XpdProj = project(tangent, binormal, cache.position.xyz, Xpd);
	vec3 XpgProj = project(tangent, binormal, cache.position.xyz, Xpg);
	
	vec3 Sd = sqrt(max(Xpdq - (Xpd * Xpd), vec3(EPSILON)));
	vec3 Sg = sqrt(max(Xpgq - (Xpg * Xpg), vec3(EPSILON)));
	
	float Rd = max(Sd.x, max(Sd.y, Sd.z));
	float Rg = max(Sg.x, max(Sg.y, Sg.z));
	
	float Fpd = max(calculateProjectedProxyLightArea(cache.position.xyz, cache.normal.xyz, Xpd, Rd), 0.0);
	float Fpg = max(calculateProjectedProxyLightArea(cache.position.xyz, cache.normal.xyz, Xpg, Rg), 0.0);
	
	float diffuseSquareSideLength = sqrt(Fpd) * 0.5;
	float specularSquareSideLength = sqrt(Fpg) * 0.5;

	vec2 areaHeuristicMinD = vec2(INT32_MAX), areaHeuristicMaxD = vec2(-INT32_MAX);
	vec2 areaHeuristicMinG = vec2(INT32_MAX), areaHeuristicMaxG = vec2(-INT32_MAX);
	
	for(uint y = 0; y < cacheBufferResolution; y++)
	{
		if(y * cacheBufferResolution >= cacheNumber)
		{
			break;
		}
		
		for(uint x = 0; x < cacheBufferResolution; x++)
		{
			if(y * cacheBufferResolution + x >= cacheNumber)
			{
				break;
			}
			
			ivec2 texCoord = ivec2(x, y);

			if(texCoord != ivec2(gl_FragCoord.xy))
			{
				vec3 cachePos = texelFetch(globalCachePositionBuffer, texCoord, 0).xyz;
				vec3 cacheNormal = normalize(decodeNormal(texelFetch(globalCacheNormalBuffer, texCoord, 0).xy));
				float cacheArea = texelFetch(globalCacheAreaBuffer, texCoord, 0).r;
				
				float cacheAABBSideLength = 0.5 * sqrt(calculateProjectedCacheArea(cache.position.xyz, cache.normal.xyz, cachePos, cacheNormal, cacheArea));
				
				vec3 cacheProjPos = project(tangent, binormal, cache.position.xyz, cachePos);
				
				float aD = overlappingArea(XpdProj, diffuseSquareSideLength, cacheProjPos, cacheAABBSideLength, tangent, binormal, areaHeuristicMinD, areaHeuristicMaxD);
				areaD += max(0.0, aD / Rd * min(Rd, dot(cachePos - Xpd, normalize(cache.position.xyz - Xpd))));
				
				float aG = overlappingArea(XpgProj, specularSquareSideLength, cacheProjPos, cacheAABBSideLength, tangent, binormal, areaHeuristicMinG, areaHeuristicMaxG);
				areaG += max(0.0, aG / Rg * min(Rg, dot(cachePos - Xpg, normalize(cache.position.xyz - Xpg))));
			}
		}
	}
	
	areaHeuristicMinD = max(areaHeuristicMinD, vec2(-diffuseSquareSideLength, -diffuseSquareSideLength));
	areaHeuristicMaxD = min(areaHeuristicMaxD, vec2(diffuseSquareSideLength, diffuseSquareSideLength));

	areaHeuristicMinG = max(areaHeuristicMinG, vec2(-specularSquareSideLength, -specularSquareSideLength));
	areaHeuristicMaxG = min(areaHeuristicMaxG, vec2(specularSquareSideLength, specularSquareSideLength));
	
	vec2 diffD = areaHeuristicMaxD - areaHeuristicMinD;
	float proxyLightAreaD = diffD.x * diffD.y;
	//areaD = min(areaD, proxyLightAreaD);
	
	vec2 diffG = areaHeuristicMaxG - areaHeuristicMinG;
	float proxyLightAreaG = diffG.x * diffG.y;
	//areaG = min(areaG, proxyLightAreaG);
	
	visibilityD = Fpd - EPSILON > 0.0 ? max(1.0 - areaD / Fpd, 0.0) : 0.0;
	visibilityG = Fpg - EPSILON > 0.0 ? max(1.0 - areaG / Fpg, 0.0) : 0.0;
}

void main()
{
	if(texture(zBuffer, gsout_texCoord).r == 0) //discard the indirect lighting where the cache is being occluded
	{
		return;
	}

	vec3 Lod = vec3(0.0), Xpd = vec3(0.0), Xpdq = vec3(0.0);
	vec3 Log = vec3(0.0), Xpg = vec3(0.0), Xpgq = vec3(0.0);
	
	float Wgesd = 0.0;
	float Wgesg = 0.0;
	
	CacheData cache;
	cache.position = texture(globalCachePositionBuffer, gsout_texCoord);
	vec4 cacheNormalMaterialData = texture(globalCacheNormalBuffer, gsout_texCoord);
	cache.normal.xyz = normalize(decodeNormal(cacheNormalMaterialData.xy));
	
	cache.position.w = 1.0;
	//cacheNormalMaterialData.zw = vec2(1.0, 32.0);
	
	float frd = cache.position.w / PI;
	float frg = cacheNormalMaterialData.z * (cacheNormalMaterialData.w + 2.0) / (2.0 * PI);
	
	vec3 camDir = normalize(eyePos.xyz - cache.position.xyz);

	for(uint i = 0; i < reflectiveShadowMapNumber; i++)
	{
		vec4 projPosition = reflectiveShadowLight[i].lightViewProj * vec4(cache.position.xyz, 1.0);
		projPosition /= projPosition.w;
		projPosition.xy = clamp(projPosition.xy * 0.5 + 0.5, 0.0, 1.0);
		
		for(uint j = 0; j < SAMPLERNUMBER; j++)
		{
			vec3 texCoords;
			texCoords.x = projPosition.x + samples[j].x;
			texCoords.y = projPosition.y + samples[j].y;
			texCoords.z = float(i) / float(reflectiveShadowMapNumber);
			
			vec4 lightPos = texture(indirectLightPosSampler, texCoords, 0);
			
			if(lightPos.w == 0.0) continue;//cancels indirect lightsources which aren't initialized, because there is no geometry behind
			
			vec4 lightNormalArea = texture(indirectLightNormalSampler, texCoords, 0);
			
			vec3 lightNormal = normalize(lightNormalArea.xyz * 2.0 - 1.0);
			float lightArea = lightNormalArea.w;
			
			vec3 luminousFlux = texture(indirectLightLuminousFluxSampler, texCoords, 0).rgb;
			
			vec3 lightDir = lightPos.xyz - cache.position.xyz;
			float quadDistance = dot(lightDir, lightDir);
			lightDir = normalize(lightDir);
			
			float reflAngle = max(pow(dot(reflect(-lightDir, cache.normal.xyz), camDir), cacheNormalMaterialData.w), 0.00001);
			//vec3 halfVector = (lightDir + camDir) / length(lightDir + camDir);
			//float reflAngle = max(pow(dot(halfVector, cache.normal.xyz), cacheNormalMaterialData.w), 0.00001);
			
			float F = max(dot(lightNormal.xyz, normalize(cache.position.xyz - lightPos.xyz)) * dot(cache.normal.xyz, lightDir), 0) / (lightArea + PI * quadDistance);
			
			float Wd = F;
			float Wg = reflAngle * F;
			
			Xpd += Wd * lightPos.xyz;
			Xpg += Wg * lightPos.xyz;
			
			Xpdq += Wd * lightPos.xyz * lightPos.xyz;
			Xpgq += Wg * lightPos.xyz * lightPos.xyz;
			
			Lod += luminousFlux * frd * F;
			Log += luminousFlux * frg * reflAngle * F;
			
			Wgesd += Wd;
			Wgesg += Wg;
		}
	}
	
	Xpd = Wgesd > 0 ? Xpd / Wgesd : vec3(0);
	Xpg = Wgesg > 0 ? Xpg / Wgesg : vec3(0);
	
	Xpdq = Wgesd > 0 ? Xpdq / Wgesd : vec3(0);
	Xpgq = Wgesg > 0 ? Xpgq / Wgesg : vec3(0);
		
	float visibilityD, visibilityG;
	calculateVisibility(cache, Xpd, Xpg, Xpdq, Xpgq, visibilityD, visibilityG);
	
	vec3 lightDirD = Xpd - cache.position.xyz;
	float lightAngleD = max(dot(cache.normal.xyz, normalize(lightDirD)), 0.1);

	indirectLightPositionD = vec4(Xpd, length(lightDirD));
	indirectLightLuminousFluxD = visibilityD * vec4((4.0 * PI * Lod * dot(lightDirD, lightDirD)) / (frd * lightAngleD), 0);
	
	vec3 lightDirG = Xpg - cache.position.xyz;
	float lightQuadDistance = dot(lightDirG, lightDirG);
	lightDirG = normalize(lightDirG);
	float reflAngleG = max(pow(dot(reflect(-lightDirG, cache.normal.xyz), camDir), cacheNormalMaterialData.w), 0.1);
	//vec3 halfVector = (lightDirG + camDir) / length(lightDirG + camDir);
	//float reflAngleG = max(pow(dot(halfVector, cache.normal.xyz), cacheNormalMaterialData.w), 0.1);
	float lightAngleG = max(dot(lightDirG, cache.normal.xyz), 0.1);
	
	indirectLightPositionG = vec4(Xpg, sqrt(lightQuadDistance));
	indirectLightLuminousFluxG = visibilityG * vec4((4.0 * PI * Log * lightQuadDistance) / (frg * reflAngleG * lightAngleG), 0);
}
#version 440 core

#define SAMPLENUMBER
#define PI 3.14159265359f
#define EPSILON 0.00001f
#define INT32_MAX 2147483647

#include "../../../include/Shader/LightData.glslh"
#include "../../../include/Shader/IndirectLightData.glslh"
#include "../../../include/Shader/CacheData.glslh"
#include "../../../include/Shader/CameraUBO.glslh"
#include "../../../include/Shader/Encodings.glslh"

layout(binding = 1) uniform samplingPattern
{
	vec4 samples[SAMPLENUMBER];
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
	float cosAngle1 = max(dot(cacheNormal1, normalize(-cacheDir)), 0.0);
	
	return max((cacheArea * cosAngle0 * cosAngle1) / (cacheArea + PI * dot(cacheDir, cacheDir)), 0.0);//no negative are if cache lies behind the other cache!
}

float overlappingArea(in vec3 p0, in float length0, in vec3 p1, in float length1, vec3 t, vec3 b)
{
	vec3 diff = p1 - p0;
	
	float shareT = abs(dot(diff, t));
	float shareB = abs(dot(diff, b));
	
	float minLength = min(length0, length1) * 2.0;//SHOULDNT WE USE HERE THE FULL SIDE LENGTH; NOT ONLY THE HALF?!
	float ax = min(abs(min(shareT - (length0 + length1), 0.0)), minLength);
	float ay = min(abs(min(shareB - (length0 + length1), 0.0)), minLength);
	
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
	
	vec3 Sd = sqrt(Xpdq - (Xpd * Xpd));
	vec3 Sg = sqrt(Xpgq - (Xpg * Xpg));
	
	float Rd = max(Sd.x, max(Sd.y, Sd.z));
	float Rg = max(Sg.x, max(Sg.y, Sg.z));
	
	float Fpd = calculateProjectedProxyLightArea(cache.position.xyz, cache.normal.xyz, Xpd, Rd);
	float Fpg = calculateProjectedProxyLightArea(cache.position.xyz, cache.normal.xyz, Xpg, Rg);
	
	float diffuseSquareSideLength = sqrt(Fpd) * 0.5;
	float specularSquareSideLength = sqrt(Fpg) * 0.5;
	
	ivec2 actualTexCoord = ivec2(gl_FragCoord.xy * cacheBufferResolution);
	
	for(uint y = 0; y < cacheBufferResolution; y++)
	{
		if(y * cacheBufferResolution >= cacheNumber)
		{
			break;
		}
		
		for(uint x = 0; x < cacheBufferResolution; x++)
		{
			ivec2 texCoord = ivec2(x, y);
			if(y * cacheBufferResolution + x >= cacheNumber)
			{
				break;
			}
			
			if(texCoord != actualTexCoord)
			{
				vec3 cachePos = texelFetch(globalCachePositionBuffer, texCoord, 0).xyz;
				vec3 cacheNormal = normalize(decodeNormal(texelFetch(globalCacheNormalBuffer, texCoord, 0).xy));
				float cacheArea = texelFetch(globalCacheAreaBuffer, texCoord, 0).r;
				
				float cacheAABBSideLength = 0.5 * sqrt(calculateProjectedCacheArea(cache.position.xyz, cache.normal.xyz, cachePos, cacheNormal, cacheArea));
				
				vec3 cacheProjPos = project(tangent, binormal, cache.position.xyz, cachePos);
				areaD += overlappingArea(XpdProj, diffuseSquareSideLength, cacheProjPos, cacheAABBSideLength, tangent, binormal);
				areaG += overlappingArea(XpgProj, specularSquareSideLength, cacheProjPos, cacheAABBSideLength, tangent, binormal);
			}
		}
	}
	
	visibilityD = max(1.0 - areaD / Fpd, 0.0);
	visibilityG = max(1.0 - areaG / Fpg, 0.0);
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
	
	float frd = cache.position.w / PI;
	float frg = cacheNormalMaterialData.z * (cacheNormalMaterialData.w + 8.0) / (8.0 * PI);
	
	vec3 camDir = normalize(eyePos.xyz - cache.position.xyz);

	for(uint i = 0; i < reflectiveShadowMapNumber; i++)
	{
		vec2 projPosition = clamp((reflectiveShadowLight[i].lightViewProj * cache.position).xy * 0.5 + 0.5, 0.0, 1.0);
		
		for(uint j = 0; j < SAMPLENUMBER; j++)
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
			
			vec3 luminousFlux = samples[j].z * texture(indirectLightLuminousFluxSampler, texCoords, 0).rgb;
			
			vec3 lightDir = lightPos.xyz - cache.position.xyz;
			float quadDistance = dot(lightDir, lightDir);
			lightDir = normalize(lightDir);
			
			float reflAngle = max(pow(dot(reflect(-lightDir, cache.normal.xyz), camDir), cacheNormalMaterialData.w), 0.00001);
			//vec3 halfVector = (lightDir + camDir) / length(lightDir + camDir);
			//float reflAngle = max(pow(dot(halfVector, cache.normal.xyz), cacheNormalMaterialData.w), 0.00001);
			
			float F = max(dot(lightNormal.xyz, normalize(cache.position.xyz - lightPos.xyz)) * dot(cache.normal.xyz, lightDir), 0) / (lightArea + PI * quadDistance);
			
			float Wd = EPSILON + F;
			float Wg = EPSILON + reflAngle * F;
			
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
	
	Xpd /= Wgesd;
	Xpg /= Wgesg;
	
	Xpdq /= Wgesd;
	Xpgq /= Wgesg;
	
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
#version 440 core

#define PI 3.14159265359f
#define Ep 0.0001f

#include "../../../include/Shader/LightData.glslh"
#include "../../../include/Shader/IndirectLightData.glslh"
#include "../../../include/Shader/CacheData.glslh"
#include "../../../include/Shader/CameraUBO.glslh"
#include "../../../include/Shader/Encodings.glslh"

layout(location = 0) out vec4 indirectLightPositionD;
layout(location = 1) out vec4 indirectLightLuminousFluxD;
layout(location = 2) out vec4 indirectLightPositionG;
layout(location = 3) out vec4 indirectLightLuminousFluxG;

layout(location = 0) uniform sampler2DArray indirectLightPosSampler;
layout(location = 1) uniform sampler2DArray indirectLightNormalSampler;
layout(location = 2) uniform sampler2DArray indirectLightLuminousFluxSampler;

layout(location = 3) uniform sampler2D globalCachePositionBuffer;
layout(location = 4) uniform sampler2D globalCacheNormalBuffer;

layout(location = 5) uniform sampler2D zBuffer;

layout(location = 6) uniform uint reflectiveShadowMapWidth;
layout(location = 7) uniform uint reflectiveShadowMapNumber;

layout(location = 8) uniform uint unusedLightNumber;

in vec2 gsout_texCoord;

void main()
{
	if(texture(zBuffer, gsout_texCoord).r == 0) //discard the indirect lighting e.g. for skybox or billboards
	{
		return;
	}
	
	vec3 Lod = vec3(0.0f), Xpd = vec3(0.0f);
	vec3 Log = vec3(0.0f), Xpg = vec3(0.0f);
	
	float Wgesd = 0.0f;
	float Wgesg = 0.0f;
	
	CacheData cache;
	cache.position = texture(globalCachePositionBuffer, gsout_texCoord);
	cache.normal = texture(globalCacheNormalBuffer, gsout_texCoord);
	
	float frd = cache.position.w / PI;
	float frg = cache.normal.z * (cache.normal.w + 8.0f) / (8.0f * PI);
	
	vec3 camDir = normalize(eyePos.xyz - cache.position.xyz);
	
	vec3 cacheNormal = normalize(decodeNormal(cache.normal.xy));
	
	uint lightNumber = reflectiveShadowMapWidth * reflectiveShadowMapWidth * reflectiveShadowMapNumber;
	
	for(uint j = 0; j < lightNumber; j += unusedLightNumber)
	{
		ivec3 texCoords;
		texCoords.x = int(j % reflectiveShadowMapWidth);
		texCoords.y = int((j % (reflectiveShadowMapWidth * reflectiveShadowMapWidth)) / float(reflectiveShadowMapWidth));
		texCoords.z = int(j / float(reflectiveShadowMapWidth * reflectiveShadowMapWidth));
		
		vec4 lightPos = texelFetch(indirectLightPosSampler, texCoords, 0);
		
		if(lightPos.w == 0.0f) continue;
		
		vec4 lightNormalArea = texelFetch(indirectLightNormalSampler, texCoords, 0);
		
		vec3 lightNormal = normalize(lightNormalArea.xyz * 2.0f - 1.0f);
		float lightArea = lightNormalArea.w;
		
		vec3 luminousFlux = texelFetch(indirectLightLuminousFluxSampler, texCoords, 0).rgb;
		
		vec3 lightDir = lightPos.xyz - cache.position.xyz;
		float quadDistance = dot(lightDir, lightDir);
		lightDir = normalize(lightDir);
		
		float reflAngle = max(pow(dot(reflect(-lightDir, cacheNormal), camDir), cache.normal.w), 0.00001f);
		//vec3 halfVector = (lightDir + camDir) / length(lightDir + camDir);
		//float reflAngle = max(pow(dot(halfVector, cacheNormal), cache.normal.w), 0.00001f);
	
		float F = max(dot(lightNormal.xyz, normalize(cache.position.xyz - lightPos.xyz)) * dot(cacheNormal, lightDir), 0) / (lightArea + PI * quadDistance);
		
		float Wd = Ep + F;
		float Wg = Ep + reflAngle * F;
		
		Xpd += Wd * lightPos.xyz;
		Xpg += Wg * lightPos.xyz;
		
		Lod += luminousFlux * frd * F;
		Log += luminousFlux * frg * reflAngle * F;
		
		Wgesd += Wd;
		Wgesg += Wg;
	}
	
	Xpd /= Wgesd;
	Xpg /= Wgesg;
	
	vec3 lightDirD = Xpd - cache.position.xyz;
	float lightAngleD = max(dot(cacheNormal, normalize(lightDirD)), 0.1f);

	indirectLightPositionD = vec4(Xpd, 0);
	indirectLightLuminousFluxD = vec4((4.0f * PI * Lod * dot(lightDirD, lightDirD)) / (frd * lightAngleD), 0);
	
	vec3 lightDirG = Xpg - cache.position.xyz;
	float lightQuadDistance = dot(lightDirG, lightDirG);
	lightDirG = normalize(lightDirG);
	float reflAngleG = max(pow(dot(reflect(-lightDirG, cacheNormal), camDir), cache.normal.w), 0.1f);
	//vec3 halfVector = (lightDirG + camDir) / length(lightDirG + camDir);
	//float reflAngleG = max(pow(dot(halfVector, cacheNormal), cache.normal.w), 0.1f);
	float lightAngleG = max(dot(lightDirG, cacheNormal), 0.1f);
	
	indirectLightPositionG = vec4(Xpg, 0);
	indirectLightLuminousFluxG = vec4((4.0f * PI * Log * lightQuadDistance) / (frg * reflAngleG * lightAngleG), 0);
}
#version 440 core

#define SAMPLENUMBER
#define KCACHE
#define PI 3.14159265359
#define PROXYLIGHTTEXTURERESOLUTION 256

#include "../../HydroxShader/include/Shader/Shaderincludes/LightData.glslh"
#include "../../HydroxShader/include/Shader/Shaderincludes/IndirectLightData.glslh"
#include "../../HydroxShader/include/Shader/Shaderincludes/CacheData.glslh"
#include "../../HydroxShader/include/Shader/Shaderincludes/CameraUBO.glslh"
#include "../../HydroxShader/include/Shader/Shaderincludes/Encodings.glslh"

layout(location = 0) uniform sampler2DArray indirectLightPosSampler;
layout(location = 1) uniform sampler2DArray indirectLightNormalAreaSampler;
layout(location = 2) uniform sampler2DArray indirectLightLuminousFluxSampler;

layout(location = 5) uniform sampler2D proxyLightPositionSampler;
layout(location = 6) uniform sampler2D proxyLightLuminousFluxSampler;

layout(location = 3) uniform uint reflectiveShadowMapNumber;
layout(location = 4) uniform uint cacheSize;
layout(location = 7) uniform uint valOffset;

layout(std430, binding = 0) buffer samplingPattern
{
	vec4 samples[SAMPLENUMBER];
};

layout(std430, binding = 1) buffer specularCachePositionBuffer
{
	vec4 specularCachePositions[];
};

layout(std430, binding = 2) buffer specularCacheNormalMaterialBuffer
{
	vec4 specularCacheNormals[];
};

layout(r32f, binding = 0) readonly uniform image2D edgeCacheOffsetSampler;

in vec2 gsout_texCoord;

layout(location = 0) out vec4 fsout_specularProxyLightPositionBuffer;
layout(location = 1) out vec4 fsout_specularProxyLightLuminousFluxBuffer;

void main()
{
	const uint cacheNumber = uint(imageLoad(edgeCacheOffsetSampler, ivec2(width - 1, height - 1)).r);
	
	vec3 Xpg = vec3(0.0);
	vec3 Log = vec3(0.0);
	float Wgesg = 0.0;
	
	uint cacheIndex = uint(gl_FragCoord.x) + uint(gl_FragCoord.y) * PROXYLIGHTTEXTURERESOLUTION;
	
	CacheData cache;
	cache.position = specularCachePositions[cacheIndex];
	vec4 cacheNormal = specularCacheNormals[cacheIndex];
	cache.normal.xyz = normalize(cacheNormal.xyz);

	vec2 cacheMaterial = vec2(cache.position.w, cacheNormal.w);
	
	float frg = cacheMaterial.x * (cacheMaterial.y + 2.0) / (2.0 * PI);
	
	vec3 camDir = normalize(eyePos.xyz - cache.position.xyz);

	uint validVALNumber = min(valOffset + cacheSize, SAMPLENUMBER);
	
	float validLightSamples = 0.0;
	
	//for(uint k = 0; k < reflectiveShadowMapNumber; k++)
	{
		for(uint j = valOffset; j < validVALNumber; j++)
		{
			vec3 texCoordsVAL;
			/*texCoordsVAL.x = 0.5 + samples[j].x;
			texCoordsVAL.y = 0.5 + samples[j].y;
			texCoordsVAL.z = float(0) / float(reflectiveShadowMapNumber);*/
			
			texCoordsVAL.x = mod(float(j), 1024) / 1024.0;
			texCoordsVAL.y = (float(j) / 1024.0) / 1024.0;
			texCoordsVAL.z = float(0.0) / float(reflectiveShadowMapNumber);
			
			vec4 lightPos = texture(indirectLightPosSampler, texCoordsVAL, 0);
			vec4 lightNormalArea = texture(indirectLightNormalAreaSampler, texCoordsVAL, 0);
			vec3 luminousFlux = texture(indirectLightLuminousFluxSampler, texCoordsVAL, 0).rgb;
		
			float validSampleSwitch = max(lightPos.w, 0.0);
			
			validLightSamples += validSampleSwitch;

			vec3 lightNormal = normalize(lightNormalArea.xyz * 2.0 - 1.0);
			float lightArea = lightNormalArea.w;
			
			vec3 lightDir = lightPos.xyz - cache.position.xyz;
			float quadDistance = dot(lightDir, lightDir);
			lightDir = normalize(lightDir);
			
			float reflAngle = max(pow(dot(reflect(-lightDir, cache.normal.xyz), camDir), cacheMaterial.y), 0.0);

			float F = validSampleSwitch * (max(dot(lightNormal.xyz, normalize(cache.position.xyz - lightPos.xyz)), 0.0) * max(dot(cache.normal.xyz, lightDir), 0.0)) / (lightArea + PI * quadDistance);
			
			float Wg = reflAngle * F;
			
			Xpg += Wg * lightPos.xyz;

			Log += luminousFlux * frg * reflAngle * F;
			
			Wgesg += Wg;
		}
	}
/*
	vec4 XpgWgGes = texture(proxyLightPositionSampler, gsout_texCoord);
	vec4 LogValidGes = texture(proxyLightLuminousFluxSampler, gsout_texCoord);
	
	fsout_specularProxyLightPositionBuffer = vec4(XpgWgGes.xyz + Xpg, XpgWgGes.w + Wgesg);
	fsout_specularProxyLightLuminousFluxBuffer = vec4(LogValidGes.rgb + Log, LogValidGes.a + validLightSamples);*/
	
	fsout_specularProxyLightPositionBuffer = vec4(Xpg, Wgesg);
	fsout_specularProxyLightLuminousFluxBuffer = vec4(Log, validLightSamples);
}
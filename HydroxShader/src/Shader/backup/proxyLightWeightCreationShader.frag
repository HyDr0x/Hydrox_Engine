#version 440 core

#define AREAARRAYSIZE 25
#define SAMPLENUMBER
#define KCACHE
#define PI 3.14159265359
#define EPSILON 0.00001
#define INT32_MAX 2147483647
#define WORKGROUPSIZE 128

#include "../../../../include/Shader/Shaderincludes/LightData.glslh"
#include "../../../../include/Shader/Shaderincludes/IndirectLightData.glslh"
#include "../../../../include/Shader/Shaderincludes/CacheData.glslh"
#include "../../../../include/Shader/Shaderincludes/CameraUBO.glslh"
#include "../../../../include/Shader/Shaderincludes/Encodings.glslh"

layout(std430, binding = 1) buffer samplingPattern
{
	vec4 samples[SAMPLENUMBER];
};

layout(location = 0) uniform sampler2D indirectLightPosLuminousFluxSampler;
layout(location = 1) uniform sampler2D indirectLightNormalAreaSampler;

layout(rgba32f, binding = 0) readonly uniform imageBuffer globalCachePositionBuffer;
layout(rgba32f, binding = 1) readonly uniform imageBuffer globalCacheNormalBuffer;

layout(location = 7) uniform sampler2D diffuseProxyLightPositionBuffer;
layout(location = 8) uniform sampler2D specularProxyLightPositionBuffer;
layout(location = 9) uniform sampler2D diffuseProxyLightLuminousFluxBuffer;
layout(location = 10) uniform sampler2D specularProxyLightLuminousFluxBuffer;

layout(location = 2) uniform sampler2D zBuffer;

layout(location = 3) uniform uint reflectiveShadowMapNumber;

layout(location = 4) uniform uint cacheNumber;

layout(location = 5) uniform uint proxyLightTextureResolution;

layout(location = 6) uniform uint globalVALOffset;

in vec2 gsout_texCoord;

layout(location = 0) out vec4 fsout_diffuseProxyLightPositionBuffer;
layout(location = 1) out vec4 fsout_specularProxyLightPositionBuffer;
layout(location = 2) out vec4 fsout_diffuseProxyLightLuminousFluxBuffer;
layout(location = 3) out vec4 fsout_specularProxyLightLuminousFluxBuffer;

void main()
{
	if(texture(zBuffer, gsout_texCoord).r == 0) //discard the indirect lighting where the cache is being occluded
	{
		discard;
	}

	vec4 XpdW = texture(diffuseProxyLightPositionBuffer, gsout_texCoord);
	vec4 XpgW = texture(specularProxyLightPositionBuffer, gsout_texCoord);
	
	vec3 Lod = texture(diffuseProxyLightLuminousFluxBuffer, gsout_texCoord).rgb, Xpd = XpdW.xyz;
	vec3 Log = texture(specularProxyLightLuminousFluxBuffer, gsout_texCoord).rgb, Xpg = XpgW.xyz;
	
	float Wgesd = XpdW.w;
	float Wgesg = XpgW.w;

	uint cacheIndex = uint(gl_FragCoord.x) + uint(gl_FragCoord.y) * proxyLightTextureResolution;
	CacheData cache;
	cache.position = imageLoad(globalCachePositionBuffer, int(cacheIndex));
	vec4 cacheNormalMaterialData = imageLoad(globalCacheNormalBuffer, int(cacheIndex));
	cache.normal.xyz = normalize(decodeNormal(cacheNormalMaterialData.xy));
	
	float frd = cache.position.w / PI;
	float frg = cacheNormalMaterialData.z * (cacheNormalMaterialData.w + 2.0) / (2.0 * PI);
	
	vec3 camDir = normalize(eyePos.xyz - cache.position.xyz);

	uint validVALNumber = min(globalVALOffset + KCACHE, SAMPLENUMBER);
	
	const uint tileSize = 2;
	const uint tilePixelNumber = tileSize * tileSize;
	
	float validLightSamples = 0.0;
	
	for(uint j = globalVALOffset; j < validVALNumber; j++)
	{
		vec2 texCoords;
		texCoords.x = 0.5 + samples[j].x;
		texCoords.y = 0.5 + samples[j].y;
		//texCoords.x = float(mod(mod(j, tileSize) + tileSize * (j / tilePixelNumber), 128)) / 128.0;
		//texCoords.y = (float(mod(j, tilePixelNumber) / tileSize) + float(tileSize) * (float(j) / 128.0)) / 128.0;
		//texCoords.x = float(mod(j, 128)) / 128.0;
		//texCoords.y = (float(j) / 128.0) / 128.0;
		//texCoords.z = float(k) / float(reflectiveShadowMapNumber);
	
		vec4 lightPos = texture(indirectLightPosLuminousFluxSampler, texCoords);
		
		if(lightPos.w == 0.0) //cancels indirect lightsources which aren't initialized, because there is no geometry behind or lit
		{
			//continue;
		}

		validLightSamples++;
		
		vec3 luminousFlux = unpackLuminousFlux(lightPos.w);
		
		vec4 lightNormalArea = texture(indirectLightNormalAreaSampler, texCoords);
		
		vec3 lightNormal = normalize(lightNormalArea.xyz * 2.0 - 1.0);
		float lightArea = lightNormalArea.w;
		
		vec3 lightDir = lightPos.xyz - cache.position.xyz;
		float quadDistance = dot(lightDir, lightDir);
		lightDir = normalize(lightDir);
		
		float reflAngle = pow(max(dot(reflect(-lightDir, cache.normal.xyz), camDir), 0.0), cacheNormalMaterialData.w);
		//vec3 halfVector = (lightDir + camDir) / length(lightDir + camDir);
		//float reflAngle = max(pow(dot(halfVector, cache.normal.xyz), cacheNormalMaterialData.w), 0.00001);
		
		float F = (max(dot(lightNormal.xyz, normalize(cache.position.xyz - lightPos.xyz)), 0.0) * max(dot(cache.normal.xyz, lightDir), 0.0)) / (lightArea + PI * quadDistance);
		
		float Wd = F;
		float Wg = reflAngle * F;
		
		Xpd += Wd * lightPos.xyz;
		Xpg += Wg * lightPos.xyz;
		
		Lod += luminousFlux * frd * F;
		Log += luminousFlux * frg * reflAngle * F;
		
		Wgesd += Wd;
		Wgesg += Wg;
	}
	
	if(validLightSamples > 0)
	{
		Lod *= (float(validVALNumber - globalVALOffset) / validLightSamples);
		Log *= (float(validVALNumber - globalVALOffset) / validLightSamples);
	}
	
	fsout_diffuseProxyLightPositionBuffer = vec4(Xpd, Wgesd);
	fsout_specularProxyLightPositionBuffer = vec4(Xpg, Wgesg);
	
	fsout_diffuseProxyLightLuminousFluxBuffer = vec4(Lod, 0);
	fsout_specularProxyLightLuminousFluxBuffer = vec4(Log, 0);
}
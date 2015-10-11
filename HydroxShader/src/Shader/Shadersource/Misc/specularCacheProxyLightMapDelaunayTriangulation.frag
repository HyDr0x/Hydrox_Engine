#version 440 core

#define INT32_MAX 2147483647
#define PI 3.14159265359

#include "../../../../include/Shader/Shaderincludes/CameraUBO.glslh"
#include "../../../../include/Shader/Shaderincludes/CacheData.glslh"
#include "../../../../include/Shader/Shaderincludes/IndirectLightData.glslh"

layout(location = 0) uniform sampler2D depthSampler;
layout(location = 1) uniform sampler2D normalSampler;
layout(location = 2) uniform sampler2D materialSampler;
layout(location = 3) uniform sampler2D linearDepthSampler;

layout(rgba16ui, binding = 0) writeonly uniform uimage2D cacheIndices0;
layout(rgba16ui, binding = 1) writeonly uniform uimage2D cacheIndices1;
layout(rgba16ui, binding = 2) writeonly uniform uimage2D cacheIndices2;
layout(rgba16ui, binding = 3) writeonly uniform uimage2D cacheIndices3;
layout(rgba16ui, binding = 4) writeonly uniform uimage2D cacheIndices4;
layout(rgba16ui, binding = 5) writeonly uniform uimage2D cacheIndices5;

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

layout(location = 0) out vec4 luminousFlux;

in vec3 gsout_baryzentric;
in flat vec3 gsout_normal0;
in flat vec3 gsout_normal1;
in flat vec3 gsout_normal2;
in flat vec3 gsout_Xpg0;
in flat vec3 gsout_Xpg1;
in flat vec3 gsout_Xpg2;
in flat vec3 gsout_phiPG0;
in flat vec3 gsout_phiPG1;
in flat vec3 gsout_phiPG2;
in flat vec3 gsout_cacheProxyMinDistanceG;
in flat vec3 gsout_depth;
in flat uvec4 gsout_cacheIndices[6];

void main()
{
	vec2 texCoord = vec2(gl_FragCoord.x / float(width), gl_FragCoord.y / float(height));
	
	float depth = texture(depthSampler, texCoord).x;
	float linearDepth = texture(linearDepthSampler, texCoord).x;
	vec3 normal = normalize(texture(normalSampler, texCoord).xyz * 2.0 - 1.0);
	vec4 material = texture(materialSampler, texCoord);
	
	vec4 position3D = invViewProjectionMatrix * vec4(vec3(texCoord.xy, depth) * 2.0 - 1.0, 1.0);
	position3D /= position3D.w;
	
	vec3 lightPosition = vec3(0);
	vec3 lightPhi = vec3(0);
	float lightMinDistance = 0.0;

	float summedLength = 0.0;
	const float normalError = 0.975;//0.0;//0.975;
	const float depthError = 5.0;//9999999.0;//5.0;

	if(abs(linearDepth - gsout_depth.x) < depthError && dot(gsout_normal0, normal) > normalError)
	{
		summedLength = gsout_baryzentric.x;
	}
	
	if(abs(linearDepth - gsout_depth.y) < depthError && dot(gsout_normal1, normal) > normalError)
	{
		summedLength += gsout_baryzentric.y;
	}
	
	if(abs(linearDepth - gsout_depth.z) < depthError && dot(gsout_normal2, normal) > normalError)
	{
		summedLength += gsout_baryzentric.z;
	}
	
	vec3 interpolation = gsout_baryzentric / vec3(summedLength);
	
	if(abs(linearDepth - gsout_depth.x) < depthError && dot(gsout_normal0, normal) > normalError)
	{
		lightPosition = interpolation.x * gsout_Xpg0;
		lightPhi = interpolation.x * gsout_phiPG0;
		lightMinDistance = interpolation.x * gsout_cacheProxyMinDistanceG.x;
	}
	
	if(abs(linearDepth - gsout_depth.y) < depthError && dot(gsout_normal1, normal) > normalError)
	{
		lightPosition += interpolation.y * gsout_Xpg1;
		lightPhi += interpolation.y * gsout_phiPG1;
		lightMinDistance += interpolation.y * gsout_cacheProxyMinDistanceG.y;
	}
	
	if(abs(linearDepth - gsout_depth.z) < depthError && dot(gsout_normal2, normal) > normalError)
	{
		lightPosition += interpolation.z * gsout_Xpg2;
		lightPhi += interpolation.z * gsout_phiPG2;
		lightMinDistance += interpolation.z * gsout_cacheProxyMinDistanceG.z;
	}
	
	if(summedLength > 0.0)
	{
		vec3 camDir = normalize(eyePos.xyz - position3D.xyz);
	
		vec3 lightDirG = lightPosition - position3D.xyz;
		//float lengthG = max(dot(lightDirG, lightDirG), dot(lightMinDistance, lightMinDistance));
		float lengthG = max(length(lightDirG), lightMinDistance);
		lightDirG = normalize(lightDirG);
		float frg = material.y * max(dot(lightDirG, normal), 0.0) * pow(max(dot(reflect(-lightDirG, normal), camDir), 0.0), material.w);

		//luminousFlux = vec4(lightPosition, 1);
		//luminousFlux = vec4(lightPhi * 1.01, 1);
		//luminousFlux = vec4(0.5 + 0.5 * normal, 1);
		//luminousFlux = vec4(dot(gsout_normal0, normal), dot(gsout_normal1, normal), dot(gsout_normal2, normal), 1);
		//luminousFlux = vec4(dot(gsout_normal2, normal), 0, 0, 1);
		//luminousFlux = 0.5 + 0.5 * vec4(gsout_baryzentric.x * gsout_normal0 + gsout_baryzentric.y * gsout_normal1 + gsout_baryzentric.z * gsout_normal2, 1);
		//luminousFlux = vec4(gsout_baryzentric.x * gsout_Xpg0 + gsout_baryzentric.y * gsout_Xpg1 + gsout_baryzentric.z * gsout_Xpg2, 1);
		//luminousFlux = vec4(gsout_baryzentric.x * gsout_phiPG0 + gsout_baryzentric.y * gsout_phiPG1 + gsout_baryzentric.z * gsout_phiPG2, 1);
		//luminousFlux = vec4(0.01 * lengthG);
		//luminousFlux = vec4((frg * lightPhi) / (4.0 * PI), 1.0);
		//luminousFlux = vec4(1,0,0,0);
		luminousFlux = vec4((frg * lightPhi) / (4.0 * PI * lengthG), 1.0);
		
		return;
	}
	
	/*
	vec3 w = (vec3(1.0) - abs(vec3(linearDepth) - gsout_depth) / far) * vec3(dot(gsout_normal0, normal), dot(gsout_normal1, normal), dot(gsout_normal2, normal));
	w = gsout_baryzentric * w;
	
	float wGes = w.x + w.y + w.z;
	w /= wGes;//normalization step
	
	if(wGes > 0.6)
	{
		lightPosition = w.x * gsout_Xpg0;
		lightPhi = w.x * gsout_phiPG0;
		lightMinDistance = w.x * gsout_cacheProxyMinDistanceG.x;
		
		lightPosition += w.y * gsout_Xpg1;
		lightPhi += w.y * gsout_phiPG1;
		lightMinDistance += w.y * gsout_cacheProxyMinDistanceG.y;
		
		lightPosition += w.z * gsout_Xpg2;
		lightPhi += w.z * gsout_phiPG2;
		lightMinDistance += w.z * gsout_cacheProxyMinDistanceG.z;
		
		vec3 camDir = normalize(eyePos.xyz - position3D.xyz);
	
		vec3 lightDirG = lightPosition - position3D.xyz;
		//float lengthG = max(dot(lightDirG, lightDirG), dot(lightMinDistance, lightMinDistance));
		float lengthG = max(length(lightDirG), lightMinDistance);
		lightDirG = normalize(lightDirG);
		float frg = material.y * max(dot(lightDirG, normal), 0.0) * pow(max(dot(reflect(-lightDirG, normal), camDir), 0.0), material.w);

		//luminousFlux = vec4(lightPosition, 1);
		//luminousFlux = vec4(lightPhi * 1.01, 1);
		//luminousFlux = vec4(0.5 + 0.5 * normal, 1);
		//luminousFlux = vec4(dot(gsout_normal0, normal), dot(gsout_normal1, normal), dot(gsout_normal2, normal), 1);
		//luminousFlux = vec4(dot(gsout_normal2, normal), 0, 0, 1);
		//luminousFlux = 0.5 + 0.5 * vec4(gsout_baryzentric.x * gsout_normal0 + gsout_baryzentric.y * gsout_normal1 + gsout_baryzentric.z * gsout_normal2, 1);
		//luminousFlux = vec4(gsout_baryzentric.x * gsout_Xpg0 + gsout_baryzentric.y * gsout_Xpg1 + gsout_baryzentric.z * gsout_Xpg2, 1);
		//luminousFlux = vec4(gsout_baryzentric.x * gsout_phiPG0 + gsout_baryzentric.y * gsout_phiPG1 + gsout_baryzentric.z * gsout_phiPG2, 1);
		//luminousFlux = vec4(0.01 * lengthG);
		//luminousFlux = vec4((frg * lightPhi) / (4.0 * PI), 1.0);
		//luminousFlux = vec4(1,0,0,0);
		luminousFlux = vec4((frg * lightPhi) / (4.0 * PI * lengthG), 1.0);
		
		return;
	}*/

	imageStore(cacheIndices0, ivec2(gl_FragCoord.xy), uvec4(gsout_cacheIndices[0]));
	imageStore(cacheIndices1, ivec2(gl_FragCoord.xy), uvec4(gsout_cacheIndices[1]));
	imageStore(cacheIndices2, ivec2(gl_FragCoord.xy), uvec4(gsout_cacheIndices[2]));
	imageStore(cacheIndices3, ivec2(gl_FragCoord.xy), uvec4(gsout_cacheIndices[3]));
	imageStore(cacheIndices4, ivec2(gl_FragCoord.xy), uvec4(gsout_cacheIndices[4]));
	imageStore(cacheIndices5, ivec2(gl_FragCoord.xy), uvec4(gsout_cacheIndices[5]));
	
	luminousFlux = vec4(1, 0, 0, 0);
	return;
}
#version 440 core

#define INT32_MAX 2147483647
#define PI 3.14159265359

#include "../../HydroxShader/include/Shader/Shaderincludes/CameraUBO.glslh"
#include "../../HydroxShader/include/Shader/Shaderincludes/CacheData.glslh"
#include "../../HydroxShader/include/Shader/Shaderincludes/IndirectLightData.glslh"
#include "../../HydroxShader/include/Shader/Shaderincludes/MaterialConversion.glslh"

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

layout(location = 0) out vec4 luminousFlux;

in flat vec3 gsout_normal0;
in flat vec3 gsout_normal1;
in flat vec3 gsout_normal2;
in vec3 gsout_Xpd;
in vec3 gsout_phiPD;
in vec3 gsout_Xpg;
in vec3 gsout_phiPG;
in float gsout_cacheProxyMinDistanceD;
in float gsout_cacheProxyMinDistanceG;
in flat vec3 gsout_depth;
in flat uvec4 gsout_cacheIndices[6];

void main()
{
	vec2 texCoord = vec2(gl_FragCoord.x / float(width), gl_FragCoord.y / float(height));
	
	float depth = texture(depthSampler, texCoord).x;
	vec3 linearDepth = vec3(texture(linearDepthSampler, texCoord).x);
	vec3 normal = normalize(texture(normalSampler, texCoord).xyz * 2.0 - 1.0);
	vec4 material = GGXToBlinnPhong(texture(materialSampler, texCoord));
	
	vec4 position3D = invViewProjectionMatrix * vec4(vec3(texCoord.xy, depth) * 2.0 - 1.0, 1.0);
	position3D /= position3D.w;
	
	uint summedLength = 0;
	const float normalError = 0.9975;//0.0;//0.975;
	const float depthError = 1.0;//9999999.0;//5.0;
	linearDepth = abs(linearDepth - gsout_depth);
	
	if(linearDepth.x < depthError && dot(gsout_normal0, normal) > normalError)
	{
		summedLength++;
	}
	
	if(linearDepth.y < depthError && dot(gsout_normal1, normal) > normalError)
	{
		summedLength++;
	}
	
	if(linearDepth.z < depthError && dot(gsout_normal2, normal) > normalError)
	{
		summedLength++;
	}
		
	if(summedLength == 3)
	{
		vec3 camDir = normalize(eyePos.xyz - position3D.xyz);
	
		vec3 lightDirD = gsout_Xpd - position3D.xyz;
		float lengthD = max(dot(lightDirD, lightDirD), dot(gsout_cacheProxyMinDistanceD, gsout_cacheProxyMinDistanceD));
		//float lengthD = max(length(lightDirD), gsout_cacheProxyMinDistanceD);
		float frd = material.x * max(dot(normalize(lightDirD), normal), 0.001);
	
		vec3 lightDirG = gsout_Xpg - position3D.xyz;
		//float lengthG = max(dot(lightDirG, lightDirG), dot(gsout_cacheProxyMinDistanceG, gsout_cacheProxyMinDistanceG));
		float lengthG = max(length(lightDirG), gsout_cacheProxyMinDistanceG);
		lightDirG = normalize(lightDirG);
		float frg = material.y * max(dot(lightDirG, normal), 0.0) * pow(max(dot(reflect(-lightDirG, normal), camDir), 0.0), material.w);

		//luminousFlux = vec4(gsout_Xpg, 1);
		//luminousFlux = vec4(gsout_phiPG * 1.01, 1);
		//luminousFlux = vec4(0.5 + 0.5 * normal, 1);
		//luminousFlux = vec4(dot(gsout_normal0, normal), dot(gsout_normal1, normal), dot(gsout_normal2, normal), 1);
		//luminousFlux = vec4(dot(gsout_normal2, normal), 0, 0, 1);
		//luminousFlux = 0.5 + 0.5 * vec4(gsout_baryzentric.x * gsout_normal0 + gsout_baryzentric.y * gsout_normal1 + gsout_baryzentric.z * gsout_normal2, 1);
		//luminousFlux = vec4(gsout_baryzentric.x * gsout_Xpg0 + gsout_baryzentric.y * gsout_Xpg1 + gsout_baryzentric.z * gsout_Xpg2, 1);
		//luminousFlux = vec4(gsout_baryzentric.x * gsout_phiPG0 + gsout_baryzentric.y * gsout_phiPG1 + gsout_baryzentric.z * gsout_phiPG2, 1);
		//luminousFlux = vec4(0.01 * lengthG);
		//luminousFlux = vec4((frg * gsout_phiPG) / (4.0 * PI), 1.0);
		//luminousFlux = vec4(1,0,0,0);
		//luminousFlux = vec4(max((frd * gsout_phiPD) / (4.0 * PI * lengthD), 0.0), 1.0);
		//luminousFlux = vec4(max((frg * gsout_phiPG) / (4.0 * PI * lengthG), 0.0), 1.0);
		luminousFlux = vec4(max((frd * gsout_phiPD) / (4.0 * PI * lengthD), 0.0) + max((frg * gsout_phiPG) / (4.0 * PI * lengthG), 0.0), 1.0);
		
		//return;
	}
	
	imageStore(cacheIndices0, ivec2(gl_FragCoord.xy), uvec4(gsout_cacheIndices[0]));
	imageStore(cacheIndices1, ivec2(gl_FragCoord.xy), uvec4(gsout_cacheIndices[1]));
	imageStore(cacheIndices2, ivec2(gl_FragCoord.xy), uvec4(gsout_cacheIndices[2]));
	imageStore(cacheIndices3, ivec2(gl_FragCoord.xy), uvec4(gsout_cacheIndices[3]));
	imageStore(cacheIndices4, ivec2(gl_FragCoord.xy), uvec4(gsout_cacheIndices[4]));
	imageStore(cacheIndices5, ivec2(gl_FragCoord.xy), uvec4(gsout_cacheIndices[5]));
	
	luminousFlux = vec4(0, 0, 0, 0);
	return;
}
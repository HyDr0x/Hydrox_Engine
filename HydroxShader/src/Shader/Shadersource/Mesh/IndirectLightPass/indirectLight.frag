#version 440 core

layout(early_fragment_tests) in;

#define PI 3.14159265359

#include "../../HydroxShader/include/Shader/Shaderincludes/CameraUBO.glslh"
#include "../../HydroxShader/include/Shader/Shaderincludes/MaterialConversion.glslh"

layout(location = 0) uniform sampler2D normalSampler;
layout(location = 1) uniform sampler2D materialSampler;
layout(location = 2) uniform sampler2D indirectShadowMapSampler;

out vec4 luminousFlux;

in vec3 vsout_pos3D;
in vec3 vsout_Xpd;
in vec3 vsout_phiPD;
in vec3 vsout_Xpg;
in vec3 vsout_phiPG;
in float cacheProxyMinDistanceD;
in float cacheProxyMinDistanceG;

void main()
{
	vec2 texCoord = vec2(gl_FragCoord.x / float(width), gl_FragCoord.y / float(height));
	vec3 normal = normalize(texture(normalSampler, texCoord).xyz * 2.0 - 1.0);
	vec4 material = GGXToBlinnPhong(texture(materialSampler, texCoord));
	
	vec3 camDir = normalize(eyePos.xyz - vsout_pos3D);

	vec3 lightDirD = vsout_Xpd - vsout_pos3D;
	float lengthD = max(dot(lightDirD, lightDirD), dot(cacheProxyMinDistanceD, cacheProxyMinDistanceD));
	//float lengthD = max(length(lightDirD), cacheProxyMinDistanceD);
	float frd = material.x * max(dot(normalize(lightDirD), normal), 0.001);
	
	vec3 lightDirG = vsout_Xpg - vsout_pos3D;
	//float lengthG = max(dot(lightDirG, lightDirG), dot(cacheProxyMinDistanceG, cacheProxyMinDistanceG));
	float lengthG = max(length(lightDirG), cacheProxyMinDistanceG);
	lightDirG = normalize(lightDirG);
	float frg = material.y * max(dot(lightDirG, normal), 0.0) * pow(max(dot(reflect(-lightDirG, normal), camDir), 0.0), material.w);

	//luminousFlux = vec4(normal, 1);
	//luminousFlux = vec4(vsout_phiPD * 0.01, 1);
	//luminousFlux = vec4(lengthD);
	//luminousFlux = vec4(vsout_Xpd, 1);
	//luminousFlux = vec4((frg * vsout_phiPG) / (4.0 * PI), 1.0);
	//luminousFlux = vec4((frd * vsout_phiPD) / (4.0 * PI * lengthD), 1.0);
	//luminousFlux = vec4((frg * vsout_phiPG) / (4.0 * PI * lengthG), 1.0);
	luminousFlux = vec4(max((frd * vsout_phiPD) / (4.0 * PI * lengthD), vec3(0)) + max((frg * vsout_phiPG) / (4.0 * PI * lengthG), vec3(0)), 1.0);
	//luminousFlux = texture(indirectShadowMapSampler, texCoord).r * vec4((max((frd * vsout_phiPD) / (4.0 * PI * lengthD), vec3(0)) + max((frg * vsout_phiPG) / (4 * PI * lengthG), vec3(0))), 1.0);
}
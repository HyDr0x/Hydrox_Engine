#version 440 core

#define INT32_MAX 2147483647
#define PI 3.14159265359f

#include "../../../../../include/Shader/Shaderincludes/CameraUBO.glslh"
#include "../../../../../include/Shader/Shaderincludes/IndirectLightData.glslh"
#include "../../../../../include/Shader/Shaderincludes/Encodings.glslh"

layout(location = 0) uniform sampler2D normalSampler;
layout(location = 1) uniform sampler2D materialSampler;

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
	vec4 material = texture(materialSampler, texCoord);
	
	vec3 camDir = normalize(eyePos.xyz - vsout_pos3D);

	vec3 lightDirD = vsout_Xpd - vsout_pos3D;
	float lengthD = max(length(lightDirD), cacheProxyMinDistanceD);//max(dot(lightDirD, lightDirD), cacheProxyMinDistanceD);//length(lightDirD)
	//float frd = material.x / PI;
	float frd = material.x * max(dot(normalize(lightDirD), normal), 0.0);
	
	vec3 lightDirG = vsout_Xpg - vsout_pos3D;
	float lengthG = max(length(lightDirG), cacheProxyMinDistanceG);//max(dot(lightDirG, lightDirG), cacheProxyMinDistanceG);//length(lightDirG)
	//float frg = material.y * (material.w + 2.0) / (2.0 * PI);
	lightDirG = normalize(lightDirG);
	float frg = material.y * max(dot(lightDirG, normal), 0.0) * pow(max(dot(reflect(-lightDirG, normal), camDir), 0.0), material.w);
	
	//luminousFlux = vec4(normal, 1);
	//luminousFlux = vec4(vsout_phiPD, 1);
	//luminousFlux = vec4((frg * vsout_phiPG) / (4.0 * PI), 1.0);
	//luminousFlux = vec4((frd * vsout_phiPD) / (4.0 * PI * lengthD), 1.0);
	//luminousFlux = vec4((frg * vsout_phiPG) / (4.0 * PI * lengthG), 1.0);
	luminousFlux = vec4((frd * vsout_phiPD) / (4.0 * PI * lengthD) + (frg * vsout_phiPG) / (4.0 * PI * lengthG), 1.0);
}
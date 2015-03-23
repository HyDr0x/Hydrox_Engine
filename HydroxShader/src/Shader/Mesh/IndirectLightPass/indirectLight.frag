#version 440 core

#define INT32_MAX 2147483647
#define PI 3.14159265359f

#include "../../../../include/Shader/CameraUBO.glslh"
#include "../../../../include/Shader/CacheData.glslh"
#include "../../../../include/Shader/IndirectLightData.glslh"
#include "../../../../include/Shader/Encodings.glslh"

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
	vec4 material = texture(materialSampler, texCoord);
	vec3 normal = normalize(texture(normalSampler, texCoord).xyz * 2.0f - 1.0f);//vec3(0,1,0);

	vec3 camDir = normalize(eyePos.xyz - vsout_pos3D);

	vec3 lightDirD = vsout_Xpd - vsout_pos3D;
	float lengthD = max(length(lightDirD), cacheProxyMinDistanceD);
	lightDirD = normalize(lightDirD);
	float frd = material.x * max(dot(lightDirD, normal), 0.0);
			
	vec3 lightDirG = vsout_Xpg - vsout_pos3D;
	float lengthG = max(length(lightDirG), cacheProxyMinDistanceG);
	lightDirG = normalize(lightDirG);
	vec3 reflectLightDirG = reflect(-lightDirG, normal);
	float frg = material.y * max(dot(lightDirG, normal), 0) * pow(max(dot(reflectLightDirG, camDir), 0), material.w);
	
	//luminousFlux = vec4(1, 0, 0, 1);
	//luminousFlux = vec4(vsout_Xpd, 1);
	//luminousFlux = vec4(vsout_phiPD * 1.0, 1);
	//luminousFlux = vec4(frd, 0, 0, 0);
	//luminousFlux = vec4((frd * vsout_phiPD) / (4.0f * PI * lengthD), 1.0f);
	//luminousFlux = vec4((frg * vsout_phiPG) / (4.0f * PI * lengthG), 1.0f);
	luminousFlux = vec4((frd * vsout_phiPD) / (4.0f * PI * lengthD) + (frg * vsout_phiPG) / (4.0f * PI * lengthG), 1.0f);
}
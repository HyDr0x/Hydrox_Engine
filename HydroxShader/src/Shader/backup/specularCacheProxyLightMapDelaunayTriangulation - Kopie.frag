#version 440 core

#define PI 3.14159265359

#include "../../../../include/Shader/Shaderincludes/CameraUBO.glslh"

layout(location = 0) uniform sampler2D normalSampler;
layout(location = 1) uniform sampler2D materialSampler;

layout(location = 0) out vec4 luminousFlux;

in vec3 gsout_pos3D;
in vec3 gsout_Xpg;
in vec3 gsout_phiPG;
in float gsout_cacheProxyMinDistanceG;

void main()
{
	vec2 texCoord = vec2(gl_FragCoord.x / float(width), gl_FragCoord.y / float(height));
	vec3 normal = normalize(texture(normalSampler, texCoord).xyz * 2.0 - 1.0);
	vec4 material = texture(materialSampler, texCoord);
	
	vec3 camDir = normalize(eyePos.xyz - gsout_pos3D);

	vec3 lightDirG = gsout_Xpg - gsout_pos3D;
	//float lengthG = max(dot(lightDirG, lightDirG), dot(gsout_cacheProxyMinDistanceG, gsout_cacheProxyMinDistanceG));
	float lengthG = max(length(lightDirG), gsout_cacheProxyMinDistanceG);
	lightDirG = normalize(lightDirG);
	float frg = material.y * max(dot(lightDirG, normal), 0.0) * pow(max(dot(reflect(-lightDirG, normal), camDir), 0.0), material.w);

	//luminousFlux = vec4(normal, 1);
	//luminousFlux = vec4(gsout_phiPG * 0.01, 1);
	//luminousFlux = vec4(lengthG);
	//luminousFlux = vec4(gsout_Xpg, 1);
	//luminousFlux = vec4((frg * gsout_phiPG) / (4.0 * PI), 1.0);
	//luminousFlux = vec4((frg * gsout_phiPG) / (4.0 * PI * lengthG), 1.0);
	luminousFlux = vec4(max((frg * gsout_phiPG) / (4 * PI * lengthG), vec3(0)), 1.0);
}
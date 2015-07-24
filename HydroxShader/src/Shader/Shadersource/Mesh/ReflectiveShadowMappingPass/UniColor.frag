#version 440 core

layout(early_fragment_tests) in;

#define SAMPLENUMBER

#include "../../../../../include/Shader/Shaderincludes/MaterialData.glslh"
#include "../../../../../include/Shader/Shaderincludes/LightData.glslh"
#include "../../../../../include/Shader/Shaderincludes/Encodings.glslh"

layout(location = 0) out vec4 fsout_pos3DLuminousFlux;
layout(location = 1) out vec4 fsout_normalArea;

layout(location = 0) uniform sampler2D colorSampler;

layout(std430, binding = 4) buffer reflectiveShadowLightBuffer
{
	ShadowLightData reflectiveShadowLight[];
};

layout(location = 2) uniform int lightIndex;
layout(location = 3) uniform uint shadowMapWidth;

in vec4 vsout_pos;
in vec3 vsout_normal;
flat in uint vsout_instanceIndex;

void main()
{
	vec4 projPar = reflectiveShadowLight[lightIndex].light.projectionParameter;//x = near, y = far, z = width, w = height
	float area;
	float lightAngle;
	vec4 lightPos = reflectiveShadowLight[lightIndex].light.position;

	if(lightPos.x != DIRECTIONAL_LIGHT_POSITION)//spotlight
	{
		lightAngle = max(dot(normalize(-reflectiveShadowLight[lightIndex].light.direction.xyz), normalize(vsout_pos.xyz - lightPos.xyz)), 0.0);
		area = (gl_FragCoord.z * gl_FragCoord.z * projPar.z * projPar.w) / (projPar.x * projPar.x * SAMPLENUMBER);
	}
	else//directional light
	{
		lightAngle = 1.0;
		area = (projPar.z * projPar.w) / SAMPLENUMBER;
	}
	
	vec3 luminousFlux = ((1.0 / float(SAMPLENUMBER)) * reflectiveShadowLight[lightIndex].light.color * reflectiveShadowLight[lightIndex].light.luminousFlux * material[materialIndex[vsout_instanceIndex]].color).xyz;
	fsout_pos3DLuminousFlux = vec4(vsout_pos.xyz, packLuminousFlux(luminousFlux));
	fsout_normalArea = vec4(vsout_normal * 0.5f + 0.5f, area);

	//fsout_luminousFlux = vec4(luminousFlux, step(reflectiveShadowLight[lightIndex].light.direction.w, lightAngle));
}
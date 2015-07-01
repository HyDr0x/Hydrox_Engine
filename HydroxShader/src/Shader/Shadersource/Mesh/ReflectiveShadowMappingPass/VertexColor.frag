#version 440 core

layout(early_fragment_tests) in;

#define PI 3.14159265359
#define SAMPLENUMBER

#include "../../../../../include/Shader/Shaderincludes/MaterialData.glslh"
#include "../../../../../include/Shader/Shaderincludes/LightData.glslh"

layout(location = 0) out vec4 fsout_pos3D;
layout(location = 1) out vec4 fsout_normal;
layout(location = 2) out vec4 fsout_luminousFlux;

layout(location = 0) uniform sampler2D colorSampler;

layout(std430, binding = 4) buffer reflectiveShadowLightBuffer
{
	ShadowLightData reflectiveShadowLight[];
};

layout(location = 2) uniform int lightIndex;
layout(location = 3) uniform uint shadowMapWidth;

in vec4 vsout_pos;
in vec3 vsout_normal;
in vec4 vsout_color;
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
		area = (gl_FragCoord.z * gl_FragCoord.z * projPar.z * projPar.w) / (projPar.x * projPar.x * SAMPLENUMBER * SAMPLENUMBER);
	}
	else//directional light
	{
		lightAngle = 1.0;
		area = (projPar.z * projPar.w) / SAMPLENUMBER;
	}
	
	fsout_pos3D = vec4(vsout_pos.xyz, 1.0f);
	fsout_normal = vec4(vsout_normal * 0.5f + 0.5f, area);

	vec3 luminousFlux = ((1.0 / float(SAMPLENUMBER)) * reflectiveShadowLight[lightIndex].light.color * reflectiveShadowLight[lightIndex].light.luminousFlux * vsout_color).xyz;
	fsout_luminousFlux = vec4(luminousFlux, step(reflectiveShadowLight[lightIndex].light.direction.w, lightAngle));
}
#version 440 core

layout(early_fragment_tests) in;

#define PI 3.14159265359
#define SAMPLENUMBER

#include "../../HydroxShader/include/Shader/Shaderincludes/MaterialData.glslh"
#include "../../HydroxShader/include/Shader/Shaderincludes/LightData.glslh"
#include "../../HydroxShader/include/Shader/Shaderincludes/Encodings.glslh"

layout(location = 0) out vec4 fsout_pos3D;
layout(location = 1) out vec4 fsout_normalArea;
layout(location = 2) out vec4 fsout_luminousFlux;

layout(std430, binding = 4) buffer reflectiveShadowLightBuffer
{
	ShadowLightData reflectiveShadowLight[];
};

layout(location = 4) uniform int lightIndex;
layout(location = 5) uniform uint reflectiveShadowMapWidth;

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
		float zLin = 2.0 * projPar.x * projPar.y / (projPar.y + projPar.x - gl_FragCoord.z * (projPar.y - projPar.x));
		area = (zLin * zLin * projPar.z * projPar.w) / (projPar.x * projPar.x * float(reflectiveShadowMapWidth * reflectiveShadowMapWidth));
	}
	else//directional light
	{
		lightAngle = 1.0;
		area = (projPar.z * projPar.w) / SAMPLENUMBER;
	}
	
	float attenutation = clamp((lightAngle - reflectiveShadowLight[lightIndex].light.direction.w) * 1.0 / 0.05, 0.0, 1.0);
	if(attenutation == 0.0) discard;
	
	vec3 luminousFlux = (reflectiveShadowLight[lightIndex].light.color * reflectiveShadowLight[lightIndex].light.luminousFlux * vsout_color).xyz;
	fsout_pos3D = vec4(vsout_pos.xyz, 1.0);
	fsout_luminousFlux = vec4(luminousFlux, 1.0);
	fsout_normalArea = vec4(vsout_normal * 0.5f + 0.5f, area);
}
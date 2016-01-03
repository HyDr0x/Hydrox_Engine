#version 440 core

layout(early_fragment_tests) in;

#define SAMPLENUMBER

#include "../../HydroxShader/include/Shader/Shaderincludes/MaterialData.glslh"
#include "../../HydroxShader/include/Shader/Shaderincludes/LightData.glslh"
#include "../../HydroxShader/include/Shader/Shaderincludes/Encodings.glslh"

layout(location = 0) out vec4 fsout_pos3D;
layout(location = 1) out vec4 fsout_normalArea;
layout(location = 2) out vec4 fsout_luminousFlux;

layout(std430, binding = 9) buffer reflectiveShadowLightBufferFrag
{
	ShadowLightData reflectiveShadowLightFrag[];
};

layout(location = 4) uniform int lightIndex;
layout(location = 5) uniform uint reflectiveShadowMapWidth;

in GeometryData
{
	vec3 pos;
	vec3 normal;
	flat uint instanceIndex;
} inData;

void main()
{
	vec4 projPar = reflectiveShadowLightFrag[lightIndex].light.projectionParameter;//x = near, y = far, z = width, w = height
	float area;
	float lightAngle;
	vec4 lightPos = reflectiveShadowLightFrag[lightIndex].light.position;
	float invLightDistance = 1.0;
	
	if(lightPos.x != DIRECTIONAL_LIGHT_POSITION)//spotlight
	{
		vec3 lightDir = inData.pos - lightPos.xyz;
		lightAngle = max(dot(normalize(-reflectiveShadowLightFrag[lightIndex].light.direction.xyz), normalize(lightDir)), 0.0);
		float zLin = 2.0 * projPar.x * projPar.y / (projPar.y + projPar.x - gl_FragCoord.z * (projPar.y - projPar.x));
		area = (zLin * zLin * projPar.z * projPar.w) / (projPar.x * projPar.x * float(reflectiveShadowMapWidth * reflectiveShadowMapWidth));
		
		float lightDistance = length(lightDir);
		invLightDistance = (1.0 / (lightDistance * lightDistance));
	}
	else//directional light
	{
		lightAngle = 1.0;
		area = (projPar.z * projPar.w) / SAMPLENUMBER;
	}
	
	float attenutation = clamp((lightAngle - reflectiveShadowLightFrag[lightIndex].light.direction.w) * 1.0 / 0.05, 0.0, 1.0);
	if(attenutation == 0.0) discard;

	vec3 luminousFlux = invLightDistance * (reflectiveShadowLightFrag[lightIndex].light.color * reflectiveShadowLightFrag[lightIndex].light.luminousFlux * material[materialIndex[inData.instanceIndex]].color).xyz;
	fsout_pos3D = vec4(inData.pos, 1.0);
	fsout_luminousFlux = vec4(luminousFlux, 1.0);
	fsout_normalArea = vec4(inData.normal * 0.5 + 0.5, area);
}
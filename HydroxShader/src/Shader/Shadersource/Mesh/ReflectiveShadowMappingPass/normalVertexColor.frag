#version 440 core

layout(early_fragment_tests) in;

#define SAMPLERNUMBER

#include "../../../../../include/Shader/Shaderincludes/MaterialData.glslh"
#include "../../../../../include/Shader/Shaderincludes/LightData.glslh"

layout(location = 0) out vec4 fsout_pos3D;
layout(location = 1) out vec4 fsout_normal;
layout(location = 2) out vec4 fsout_luminousFlux;

layout(location = 0) uniform sampler2D colorSampler;
layout(location = 1) uniform sampler2D normalSampler;

layout(std430, binding = 4) buffer reflectiveShadowLightBuffer
{
	ShadowLightData reflectiveShadowLight[];
};

layout(location = 2) uniform int lightIndex;
layout(location = 3) uniform uint shadowMapWidth;

in vec4 vsout_pos;
in vec2 vsout_texCoord;
in mat3 vsout_tangentToWorld;
flat in uint vsout_instanceIndex;
in vec4 vsout_color;

void main()
{
	vec4 projPar = reflectiveShadowLight[lightIndex].light.projectionParameter;//x = near, y = far, z = width, w = height
	float area;
	if(reflectiveShadowLight[lightIndex].light.position.x != DIRECTIONAL_LIGHT_POSITION)
	{
		area = (gl_FragCoord.z * gl_FragCoord.z * projPar.z * projPar.w) / (projPar.x * projPar.x * shadowMapWidth * shadowMapWidth);
	}
	else
	{
		area = 1.0f / (SAMPLERNUMBER * projPar.z * projPar.w);
	}
	
	vec3 normal = normalize(vsout_tangentToWorld * (texture(normalSampler, vsout_texCoord).xyz * 2.0f - 1.0f));
	fsout_normal = vec4(normal * 0.5f + 0.5f, area);
	
	fsout_pos3D = vec4(vsout_pos.xyz, 1.0f);
	
	fsout_luminousFlux = (1.0 / float(SAMPLERNUMBER)) * reflectiveShadowLight[lightIndex].light.color * reflectiveShadowLight[lightIndex].light.luminousFlux * material[materialIndex[vsout_instanceIndex]].diffuseStrength * vsout_color;
}
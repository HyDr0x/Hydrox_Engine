#version 440 core

layout(early_fragment_tests) in;

#include "../../Header/MaterialData.glslh"
#include "../../Header/LightData.glslh"

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
flat in uint vsout_instanceIndex;
in vec4 vsout_color;

void main()
{
	float zNear = reflectiveShadowLight[lightIndex].projectionParameter.x;
	float nearArea = reflectiveShadowLight[lightIndex].projectionParameter.y;
	float area = gl_FragCoord.z * gl_FragCoord.z * nearArea * nearArea / (zNear * zNear * shadowMapWidth * shadowMapWidth);
	
	fsout_pos3D = vsout_pos;
	fsout_normal = vec4(vsout_normal * 0.5f + 0.5f, area);

	vec3 lightDir = reflectiveShadowLight[lightIndex].light.position.xyz - vsout_pos.xyz;
	float distance = dot(lightDir, lightDir);
	lightDir = normalize(lightDir);
	
	fsout_luminousFlux = max(dot(lightDir, vsout_normal), 0.0f) * reflectiveShadowLight[lightIndex].light.luminousFlux * material[materialIndex[vsout_instanceIndex]].diffuseStrength * vsout_color / distance;
}
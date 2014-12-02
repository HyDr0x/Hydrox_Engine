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
in vec2 vsout_texCoord;
in vec3 vsout_normal;
flat in uint vsout_instanceIndex;

void main()
{
	vec3 projPar = reflectiveShadowLight[lightIndex].projectionParameter.xyz;//x = near, y = far, z = width
	float zLinear = 2.0f * projPar.x * projPar.y / (projPar.y + projPar.x - gl_FragCoord.z * (projPar.y - projPar.x));
	float area = zLinear * zLinear * projPar.z * projPar.z / (projPar.x * projPar.x * shadowMapWidth * shadowMapWidth);
	
	fsout_pos3D = vec4(vsout_pos.xyz, 0);
	fsout_normal = vec4(vsout_normal * 0.5f + 0.5f, area);
	
	vec3 lightDir = reflectiveShadowLight[lightIndex].light.position.xyz - vsout_pos.xyz;
	float distance = dot(lightDir, lightDir);
	lightDir = normalize(lightDir);
	
	float sr = area / distance;
	float cosTheta = max(dot(lightDir, vsout_normal), 0.0f);
	fsout_luminousFlux = sr * cosTheta * reflectiveShadowLight[lightIndex].light.color * reflectiveShadowLight[lightIndex].light.luminousFlux * material[materialIndex[vsout_instanceIndex]].diffuseStrength * texture(colorSampler, vsout_texCoord);
}
#version 440 core

#include "../../HydroxShader/include/Shader/Shaderincludes/CameraUBO.glslh"
#include "../../HydroxShader/include/Shader/Shaderincludes/Lights.glslh"

layout(location = 0) uniform sampler2D depthSampler;
layout(location = 1) uniform sampler2D albedoSampler;
layout(location = 2) uniform sampler2D normalSampler;
layout(location = 3) uniform sampler2D materialSampler;

layout(location = 4) uniform sampler2DArray shadowMapsSampler;
layout(location = 5) uniform sampler2DArray reflectiveShadowDepthMapsSampler;

layout(location = 6) uniform uint lightNumber;
layout(location = 7) uniform uint shadowLightNumber;
layout(location = 8) uniform uint reflectiveShadowLightNumber;

layout(std430, binding = 0) buffer lightBuffer
{
	LightData light[];
};

layout(std430, binding = 1) buffer shadowLightBuffer
{
	ShadowLightData shadowLight[];
};

layout(std430, binding = 2) buffer reflectiveShadowLightBuffer
{
	ShadowLightData reflectiveShadowLight[];
};

void calculateShadowedLight(inout vec4 luminousFlux, in const ShadowLightData incomingLight, in const uint shadowMapID, in const vec3 position, in const vec3 normal, in const vec3 albedo, in const vec4 material)
{
	vec4 shadowPos = incomingLight.lightViewProj * vec4(position, 1.0);
	shadowPos /= shadowPos.w;
	shadowPos.xyz = shadowPos.xyz * 0.5 + 0.5;
	vec3 shadowTexCoords = vec3(shadowPos.xy, shadowMapID);
	float shadowZ = texture(reflectiveShadowDepthMapsSampler, shadowTexCoords).r;
	
	if(shadowPos.z <= shadowZ + 0.0001 && (shadowTexCoords.x >= 0.0 && shadowTexCoords.x <= 1.0 && shadowTexCoords.y >= 0.0 && shadowTexCoords.y <= 1.0))
	{
		vec2 borderAttenuation = clamp(shadowTexCoords.xy * 1.0 / 0.05, 0.0, 1.0) * clamp((vec2(1.0) - shadowTexCoords.xy) * 1.0 / 0.05, 0.0, 1.0);
		luminousFlux += vec4(incomingLight.light.color.rgb * borderAttenuation.x * borderAttenuation.y * calculateLuminance(incomingLight.light, position, normal, albedo, material), 1.0);
	}
}

out vec4 luminousFlux;

in vec2 gsout_texCoord;

void main()
{	
	vec4 tmpPos = vec4(gsout_texCoord, texture(depthSampler, gsout_texCoord).r, 1.0);
	
	if(tmpPos.z == 1.0)//discard the lighting for background e.g. for skybox
	{
		luminousFlux = vec4(1.0);
		return;
	}
	
	tmpPos.xyz = tmpPos.xyz * 2.0 - 1.0;
	tmpPos = invViewProjectionMatrix * tmpPos;
	tmpPos /= tmpPos.w;
	vec3 pos = tmpPos.xyz;
	
	vec3 albedo = texture(albedoSampler, gsout_texCoord).rgb;
	vec4 material = texture(materialSampler, gsout_texCoord);
	
	vec3 normal = normalize(texture(normalSampler, gsout_texCoord).xyz * 2.0 - 1.0);//vec3(0,1,0);
	
	luminousFlux = vec4(0.0);
	
	for(uint i = 0; i < lightNumber; i++)
	{
		luminousFlux += vec4(light[i].color.rgb * calculateLuminance(light[i], pos, normal, albedo, material), 1.0);
	}
	
	for(uint i = 0; i < shadowLightNumber; i++)
	{
		calculateShadowedLight(luminousFlux, shadowLight[i], i, pos, normal, albedo, material);
	}
	
	for(uint i = 0; i < reflectiveShadowLightNumber; i++)
	{
		calculateShadowedLight(luminousFlux, reflectiveShadowLight[i], i, pos, normal, albedo, material);
	}
}
#version 440 core

#include "../../../../include/Shader/Shaderincludes/CameraUBO.glslh"
#include "../../../../include/Shader/Shaderincludes/Lights.glslh"

layout(location = 0) uniform sampler2D depthSampler;
layout(location = 1) uniform sampler2D normalSampler;
layout(location = 2) uniform sampler2D materialSampler;

layout(location = 3) uniform sampler2DArray shadowMapsSampler;
layout(location = 7) uniform sampler2D shadowPosMapsSampler[28];

layout(location = 4) uniform uint lightNumber;
layout(location = 5) uniform uint shadowLightNumber;
layout(location = 6) uniform uint reflectiveShadowLightNumber;

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

out vec4 luminousFlux;

in vec2 gsout_texCoord;

void main()
{	
	vec4 tmpPos = vec4(gsout_texCoord, texture(depthSampler, gsout_texCoord).r, 1.0);
	
	if(tmpPos.z == 1.0) //discard the lighting for background e.g. for skybox
	{
		luminousFlux = vec4(1.0);
		return;
	}
	
	tmpPos.xyz = tmpPos.xyz * 2.0 - 1.0;
	tmpPos = invViewProjectionMatrix * tmpPos;
	tmpPos /= tmpPos.w;
	vec3 pos = tmpPos.xyz;
	
	vec4 material = texture(materialSampler, gsout_texCoord);
	vec3 normal = normalize(texture(normalSampler, gsout_texCoord).xyz * 2.0 - 1.0);//vec3(0,1,0);
	
	luminousFlux = vec4(0);
	
	for(uint i = 0; i < lightNumber; i++)
	{
		luminousFlux += vec4(light[i].color.rgb, 1.0) * (material.z + calculateLuminance(light[i], pos, normal, material));
	}
	
	for(uint i = 0; i < shadowLightNumber; i++)
	{
		vec4 shadowPos = shadowLight[i].lightViewProj * vec4(pos, 1.0);
		shadowPos /= shadowPos.w;
		shadowPos.xyz = shadowPos.xyz * 0.5 + 0.5;
		vec3 shadowTexCoords = vec3(shadowPos.xy, i);
		float shadowZ = texture(shadowMapsSampler, shadowTexCoords).r;
		
		if(shadowPos.z <= shadowZ + 0.001 && (shadowTexCoords.x >= 0.0 && shadowTexCoords.x <= 1.0 && shadowTexCoords.y >= 0.0 && shadowTexCoords.y <= 1.0))
		{
			luminousFlux += vec4(shadowLight[i].light.color.rgb, 1.0) * (material.z + calculateLuminance(shadowLight[i].light, pos, normal, material));
		}
		else
		{
			luminousFlux += vec4(shadowLight[i].light.color.rgb, 1.0) * material.z;
		}
	}
	
	for(uint i = 0; i < reflectiveShadowLightNumber; i++)
	{
		vec4 projShadowPos = reflectiveShadowLight[i].lightViewProj * vec4(pos, 1.0);
		projShadowPos /= projShadowPos.w;
		//vec3 shadowTexCoords = vec3(projShadowPos.xy * 0.5 + 0.5, i);
		//vec3 shadowPos = texture(shadowPosMapsSampler, shadowTexCoords).xyz;
		
		vec2 shadowTexCoords = vec2(projShadowPos.xy * 0.5 + 0.5);
		vec3 shadowPos = texture(shadowPosMapsSampler[i], shadowTexCoords).xyz;
		
		if(length(shadowPos - pos) < 0.1 && (shadowTexCoords.x >= 0.0 && shadowTexCoords.x <= 1.0 && shadowTexCoords.y >= 0.0 && shadowTexCoords.y <= 1.0))
		{
			luminousFlux += vec4(reflectiveShadowLight[i].light.color.rgb, 1.0) * calculateLuminance(reflectiveShadowLight[i].light, pos, normal, material);
		}
	}
}
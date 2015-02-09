#version 440 core

#include "../../../include/Shader/CameraUBO.glslh"
#include "../../../include/Shader/Lights.glslh"

layout(location = 0) uniform sampler2D depthSampler;
layout(location = 1) uniform sampler2D normalSampler;
layout(location = 2) uniform sampler2D materialSampler;

layout(location = 3) uniform sampler2DArray shadowMapsSampler;

layout(location = 5) uniform uint lightNumber;
layout(location = 6) uniform uint shadowLightNumber;

layout(std430, binding = 0) buffer lightBuffer
{
	LightData light[];
};

layout(std430, binding = 1) buffer shadowLightBuffer
{
	ShadowLightData shadowLight[];
};

out vec4 luminousFlux;

in vec2 gsout_texCoord;

void main()
{	
	vec4 tmpPos = vec4(gsout_texCoord, texture(depthSampler, gsout_texCoord).r, 1.0f);
	
	if(tmpPos.z == 1.0f) //discard the lighting for background e.g. for skybox
	{
		luminousFlux = vec4(1.0f);
		return;
	}
	
	tmpPos.xyz = tmpPos.xyz * 2.0f - 1.0f;
	tmpPos = invViewProjectionMatrix * tmpPos;
	tmpPos /= tmpPos.w;
	vec3 pos = tmpPos.xyz;
	
	vec4 material = texture(materialSampler, gsout_texCoord);
	vec3 normal = normalize(texture(normalSampler, gsout_texCoord).xyz * 2.0f - 1.0f);//vec3(0,1,0);
	
	luminousFlux = vec4(0);
	
	for(uint i = 0; i < lightNumber; i++)
	{
		luminousFlux += vec4(light[i].color.rgb, 1.0f) * (material.z + calculateLightIntensity(light[i], pos, normal, material));
	}
	
	for(uint i = 0; i < shadowLightNumber; i++)
	{
		vec4 shadowPos = shadowLight[i].lightViewProj * vec4(pos, 1.0f);
		shadowPos /= shadowPos.w;
		shadowPos.xyz = shadowPos.xyz * 0.5f + 0.5f;
		vec3 shadowTexCoords = vec3(shadowPos.xy, i);
		float shadowZ = texture(shadowMapsSampler, shadowTexCoords).r;
		
		if(shadowPos.z <= shadowZ + 0.001 && (shadowTexCoords.x >= 0.0f && shadowTexCoords.x <= 1.0f && shadowTexCoords.y >= 0.0f && shadowTexCoords.y <= 1.0f))
		{
			luminousFlux += vec4(shadowLight[i].light.color.rgb, 1.0f) * calculateLightIntensity(shadowLight[i].light, pos, normal, material);
		}
		/*
		if(shadowPos.z <= shadowZ + 0.001 || (shadowTexCoords.x < 0.0f || shadowTexCoords.x > 1.0f || shadowTexCoords.y < 0.0f || shadowTexCoords.y > 1.0f))
		{
			luminousFlux += vec4(shadowLight[i].light.color.rgb, 1.0f) * (material.z + calculateLightIntensity(shadowLight[i].light, pos, normal, material));
		}
		else
		{
			luminousFlux += vec4(shadowLight[i].light.color.rgb, 1.0f) * material.z;
		}*/
	}
}
#version 440 core

#include "Header/CameraUBO.glslh"
#include "Header/Lights.glslh"

layout(location = 0) uniform sampler2D depthSampler;
layout(location = 1) uniform sampler2D normalSampler;
layout(location = 2) uniform sampler2D materialSampler;

layout(location = 3) uniform sampler2DArray shadowMapsSampler;
layout(location = 4) uniform sampler2DArray shadowPosSampler;

layout(location = 5) uniform uint lightNumber;
layout(location = 6) uniform uint shadowLightNumber;
layout(location = 7) uniform uint reflectiveShadowLightNumber;

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

out vec4 lightIntensity;

in vec2 gsout_texCoord;

void main()
{
	vec4 material = texture(materialSampler, gsout_texCoord);
	vec3 normal = normalize(texture(normalSampler, gsout_texCoord).xyz * 2.0f - 1.0f);//vec3(0,1,0);
	
	vec4 tmpPos = vec4(gsout_texCoord, texture(depthSampler, gsout_texCoord).r, 1.0f);
	
	if(tmpPos.z == 1.0f) //discard the lighting e.g. for skybox or billboards
	{
		lightIntensity = vec4(1.0f);
		return;
	}
	
	tmpPos.xyz = tmpPos.xyz * 2.0f - 1.0f;
	tmpPos = invViewProjectionMatrix * tmpPos;
	tmpPos /= tmpPos.w;
	vec3 pos = tmpPos.xyz;
	
	for(uint i = 0; i < lightNumber; i++)
	{
		lightIntensity += vec4(light[i].color.rgb, 1.0f) * (material.z + calculateLightIntensity(light[i], pos, normal, material));
	}
	
	for(uint i = 0; i < shadowLightNumber; i++)
	{
		vec4 shadowPos = shadowLight[i].lightViewProj * vec4(pos, 1.0f);
		shadowPos /= shadowPos.w;
		shadowPos.xyz = shadowPos.xyz * 0.5f + 0.5f;
		vec3 shadowTexCoords = vec3(shadowPos.xy, i);
		float shadowZ = texture(shadowMapsSampler, shadowTexCoords).r;
		
		if(shadowPos.z <= shadowZ + 0.001 /*+ 0.000138f 0.00007f 0.0135f*/ || (shadowTexCoords.x < 0.0f || shadowTexCoords.x > 1.0f || shadowTexCoords.y < 0.0f || shadowTexCoords.y > 1.0f))
		{
			lightIntensity += vec4(shadowLight[i].light.color.rgb, 1.0f) * (material.z + calculateLightIntensity(shadowLight[i].light, pos, normal, material));
		}
		else
		{
			lightIntensity += vec4(shadowLight[i].light.color.rgb, 1.0f) * material.z;
		}
	}
	
	for(uint i = 0; i < reflectiveShadowLightNumber; i++)
	{
		vec4 shadowPos = reflectiveShadowLight[i].lightViewProj * vec4(pos, 1.0f);
		shadowPos /= shadowPos.w;
		shadowPos.xy = shadowPos.xy * 0.5f + 0.5f;
		
		vec3 shadowTexCoords = vec3(shadowPos.xy, i);

		vec4 shadowMapPos = reflectiveShadowLight[i].lightViewProj * texture(shadowPosSampler, shadowTexCoords);
		shadowMapPos /= shadowMapPos.w;
		
		if(shadowPos.z <= shadowMapPos.z + 0.0003 /*0.001*/ || (shadowTexCoords.x < 0.0f || shadowTexCoords.x > 1.0f || shadowTexCoords.y < 0.0f || shadowTexCoords.y > 1.0f))
		{
			lightIntensity += vec4(reflectiveShadowLight[i].light.color.rgb, 1.0f) * (material.z + calculateLightIntensity(reflectiveShadowLight[i].light, pos, normal, material));
		}
		else
		{
			lightIntensity += vec4(reflectiveShadowLight[i].light.color.rgb, 1.0f) * material.z;
		}
	}
}
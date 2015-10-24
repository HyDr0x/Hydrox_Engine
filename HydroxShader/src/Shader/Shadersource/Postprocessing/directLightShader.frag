#version 440 core

#include "../../HydroxShader/include/Shader/Shaderincludes/CameraUBO.glslh"
#include "../../HydroxShader/include/Shader/Shaderincludes/Lights.glslh"

layout(location = 0) uniform sampler2D depthSampler;
layout(location = 1) uniform sampler2D albedoSampler;
layout(location = 2) uniform sampler2D normalSampler;
layout(location = 3) uniform sampler2D materialSampler;

layout(location = 4) uniform sampler2DArray shadowMapsSampler;

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
		vec4 shadowPos = shadowLight[i].lightViewProj * vec4(pos, 1.0);
		shadowPos /= shadowPos.w;
		shadowPos.xyz = shadowPos.xyz * 0.5 + 0.5;
		vec3 shadowTexCoords = vec3(shadowPos.xy, i);
		float shadowZ = texture(shadowMapsSampler, shadowTexCoords).r;
		
		if(shadowPos.z <= shadowZ + 0.0001 && (shadowTexCoords.x >= 0.0 && shadowTexCoords.x <= 1.0 && shadowTexCoords.y >= 0.0 && shadowTexCoords.y <= 1.0))
		{
			vec2 borderAttenuation = clamp(shadowTexCoords.xy * 1.0 / 0.05, 0.0, 1.0) * clamp((vec2(1.0) - shadowTexCoords.xy) * 1.0 / 0.05, 0.0, 1.0);
			luminousFlux += vec4(shadowLight[i].light.color.rgb * (borderAttenuation.x * borderAttenuation.y * calculateLuminance(shadowLight[i].light, pos, normal, albedo, material)), 1.0);
		}
	}
}
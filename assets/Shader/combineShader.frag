#version 440 core

layout(binding = 0) uniform sampler2D colorSampler;
layout(binding = 1) uniform sampler2D lightSampler;
layout(binding = 2) uniform sampler2D indirectLightSampler;

out vec4 color;

in vec2 gsout_texCoord;

void main()
{
	vec4 albedo = texture(colorSampler, gsout_texCoord);
	vec4 light = texture(lightSampler, gsout_texCoord);
	vec4 indirectLight = texture(indirectLightSampler, gsout_texCoord);
	
	color = (indirectLight + light) * albedo;
}
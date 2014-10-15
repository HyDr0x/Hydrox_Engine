#version 440 core

layout(binding = 0) uniform sampler2D colorSampler;
layout(binding = 1) uniform sampler2D lightSampler;

out vec4 color;

in vec2 gsout_texCoord;

void main()
{
	vec4 albedo = texture(colorSampler, gsout_texCoord);
	vec4 light = texture(lightSampler, gsout_texCoord);
	
	color = light * albedo;
	//color = light;
}
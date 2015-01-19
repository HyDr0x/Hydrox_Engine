#version 440 core

#include "Header/HSVRGB.glslh"

layout(location = 0) uniform sampler2D colorSampler;
layout(location = 1) uniform sampler2D lightSampler;
layout(location = 2) uniform sampler2D indirectLightSampler;

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 luminance;

in vec2 gsout_texCoord;

void main()
{
	vec4 albedo = texture(colorSampler, gsout_texCoord);
	vec4 light = texture(lightSampler, gsout_texCoord);
	vec4 indirectLight = texture(indirectLightSampler, gsout_texCoord);
	
	//vec4 gammaCorrectColor = ((indirectLight + light) * albedo) / 250.0f;
	//color = vec4(RGBtoHSV(pow(gammaCorrectColor.rgb, vec3(1.0f / 1.73f))), 1.0f);
	
	//color = (indirectLight + light) * albedo;
	
	vec4 hsvColor = vec4(RGBtoHSV(((indirectLight + light) * albedo).rgb), 1.0f);
	color = hsvColor;
	luminance = vec4(hsvColor.b, 0, 0, 0);
	
	/*vec3 normalColor = ((indirectLight + light) * albedo).rgb;
	float Y = dot(vec3(0.299f, 0.587f, 0.114f), normalColor);
	float U = (normalColor.b - Y) * 0.493f;
	float V = (normalColor.r - Y) * 0.877f;
	color = vec4(Y, U, V, 1.0f);
	luminance = vec4(Y, 0, 0, 0);*/
}
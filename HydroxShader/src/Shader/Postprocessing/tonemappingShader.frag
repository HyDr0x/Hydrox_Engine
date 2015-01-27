#version 440 core

#include "../../../include/Shader/HSVRGB.glslh"

layout(location = 0) uniform sampler2D hsvColorSampler;

layout(location = 2) uniform float s;
layout(location = 3) uniform vec2 luminanceRange;

out vec4 color;

in vec2 gsout_texCoord;

void main()
{
	vec3 hsv = texture(hsvColorSampler, gsout_texCoord).rgb;
	hsv.b = max(hsv.b - luminanceRange.x, 0);
	
	hsv.b = s * hsv.b / (s * hsv.b - hsv.b + luminanceRange.y);
	
	color = vec4(HSVtoRGB(hsv), 1.0f);
	
	/*
	vec3 yuv = texture(hsvColorSampler, gsout_texCoord).rgb;
	yuv.r = s * yuv.r / (s * yuv.r - yuv.r + maxLuminance);
	float R = yuv.r + yuv.b / 0.877f;
	float B = yuv.r + yuv.g / 0.493f;
	//float G = 1.704f * yuv.r - 0.509f * R - 0.194f * B;
	float G = yuv.r - 0.39393f * yuv.g - 0.58081f * yuv.b;
	
	color = vec4(R, G, B, 1.0f);*/
}
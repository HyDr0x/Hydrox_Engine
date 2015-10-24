#version 440 core

#include "../../HydroxShader/include/Shader/Shaderincludes/HSVRGB.glslh"

layout(location = 0) uniform sampler2D hsvColorSampler;

layout(location = 2) uniform float s;
layout(location = 3) uniform vec2 luminanceRange;

out vec4 color;

in vec2 gsout_texCoord;

void main()
{
	vec3 hsv = RGBtoHSV(texture(hsvColorSampler, gsout_texCoord).rgb);
	hsv.b = max(hsv.b - luminanceRange.x, 0);
	
	hsv.b = s * hsv.b / (s * hsv.b - hsv.b + luminanceRange.y);
	
	color = vec4(HSVtoRGB(hsv), 1.0f);
}
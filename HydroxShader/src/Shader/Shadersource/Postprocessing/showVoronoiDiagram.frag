#version 440 core

layout(location = 0) uniform usampler2D voronoiSampler;
layout(location = 1) uniform sampler1D voronoiColorSampler;

layout(location = 2) uniform uint siteNumber;

out vec4 color;

in vec2 gsout_texCoord;

void main()
{
	color = vec4(vec3(texture(voronoiColorSampler, float(texture(voronoiSampler, gsout_texCoord).r - 1) / siteNumber).rgb), 1.0);
}
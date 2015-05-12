#version 440 core

layout(early_fragment_tests) in;

layout(location = 0) out vec4 fsout_color;

layout(location = 0) uniform sampler2D colorSampler;

in vec2 vsout_texCoord;

void main()
{
	fsout_color = vec4(texture(colorSampler, vsout_texCoord).rgb, 0.0f);
}
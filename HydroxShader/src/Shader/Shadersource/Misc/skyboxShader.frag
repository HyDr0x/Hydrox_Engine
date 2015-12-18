#version 440 core

layout(early_fragment_tests) in;

layout(location = 0) uniform sampler2DArray texSampler;

layout(location = 0) out vec4 fsout_color;

in vec3 vsout_texCoord;

void main()
{
	fsout_color = vec4(texture(texSampler, vsout_texCoord).rgb, 0.0);
}
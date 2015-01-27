#version 440 core

layout(early_fragment_tests) in;

layout(location = 0) uniform sampler2D texSampler;

out vec4 color;

in vec2 vsout_texCoord;

void main()
{
	color = vec4(texture(texSampler, vsout_texCoord));
}
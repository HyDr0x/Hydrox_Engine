#version 440 core

layout(early_fragment_tests) in;

layout(location = 0) out vec4 fsout_color;

in vec4 vsout_color;

void main()
{
	fsout_color = vec4(vsout_color.rgb, 0.0f);
}
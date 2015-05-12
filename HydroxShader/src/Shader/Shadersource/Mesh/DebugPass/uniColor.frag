#version 440 core

layout(location = 0) uniform vec3 uniColor;

layout(location = 0) out vec4 fsout_color;

void main()
{
	fsout_color = vec4(uniColor, 0.0);
}